#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "scheduling.h"
#include "queue.h"

int main(int argc, char **argv) {
    FILE *processesFile;
    char *fileName;
    int opt;
    char c;

    struct process *processes;
    processes = NULL;

    clock = 0;
    int quantum = -1;
    int processTracker = 0;
    cFlag = 0; // 0 for non-challenge tasks, 1 for challenge tasks

    // store command line arguments
    while((opt = getopt(argc, argv, "f:p:c")) != -1) {
        switch(opt) {
            case 'f':
                fileName = optarg;
                processesFile = fopen(fileName, "r");
                assert(processesFile != NULL);
                break;
            case 'p':
                coreCount = atoi(optarg);
                break;
            case 'c':
                quantum = 100;
                cFlag = 1;
                break;
        }
    }

    // count processes in file
    numProcesses = 0;
    for(c = getc(processesFile); c != EOF; c = getc(processesFile)) {
        if(c == '\n') {
            numProcesses++;
        }
    }

    // store processes into array
    rewind(processesFile);
    processes = (struct process *) malloc(numProcesses * sizeof(struct process));
    assert(processes);
    for(int i = 0; i < numProcesses; i++) {
        fscanf(processesFile, "%d %d %d %c", &processes[i].timeArrived, &processes[i].processId, &processes[i].executionTime, &processes[i].parallelisable);
        processes[i].originalExecutionTime = processes[i].executionTime; // a copy of the execution time is stored to calculate performance statistics
        processes[i].processTableIndex = i;
        processes[i].subProcessFin = 9999;
    }

    // sort process table
    int exec_dup = 1;
    for(int k=0; k<numProcesses; k++) {
        if(processes[k].timeArrived == processes[k+1].timeArrived) {
            exec_dup++;
            //printf("k %d exec_dup %d\n", k, exec_dup);
            if(k == (numProcesses - 1)) {
                exec_dup--;
            }
        }
        if(processes[k].timeArrived != processes[k+1].timeArrived || (k == (numProcesses - 1))) {
            if(exec_dup > 1) {
                // printf("\nqsort %d %d\n", k-exec_dup+1, exec_dup);
                qsort(&processes[k-exec_dup+1], exec_dup, sizeof(struct process), cmpTimeArrivedExecTime);
                exec_dup = 1;
            }
        }
    }
    exec_dup = 1;
    for(int k=0; k<numProcesses; k++) {
        if(processes[k].executionTime == processes[k+1].executionTime && processes[k].timeArrived == processes[k+1].timeArrived) {
            exec_dup++;
            if(k == (numProcesses - 1)) {
                exec_dup--;
            }
        }
        if(processes[k].executionTime != processes[k+1].executionTime || (k == (numProcesses - 1))) {
            if(exec_dup > 1) {
                // printf("\nqsort %d %d\n", k-exec_dup+1, exec_dup);
                qsort(&processes[k-exec_dup+1], exec_dup, sizeof(struct process), cmpExecTimeProcessId);
                exec_dup = 1;
            }
        }
    }
    for(int i=0; i<numProcesses; i++) {
        processes[i].processTableIndex = i;
    }

    // initialise array of CPUs
    struct cpu processors[coreCount];
    for(int k=0; k<coreCount; k++) {
        processors[k].cpuQueue = (struct process *) malloc(numProcesses * sizeof(struct process));
        assert(processors[k].cpuQueue);
        processors[k].front = -1;
        processors[k].back = -1;
        processors[k].cpuRemainingTime = 0;
        processors[k].state = 0;
        processors[k].cpuRemainingExec = 0;
        processors[k].cpuId = k;
    }

    int splitCount; // stores number of times a parallelisable process is to be split
    int subTime; // stores the execution time of a sub-process
    int processesCompleted = 0;
    int shortestId = 0; // id of CPU with smallest amount of remaining execution time (fastest processor)
    int shortestRemTime = 9999; // execution time of the fastest processor
    numFin = 0;

    // simulation loop
    while(1) {
        // all processes completed
        if(processesCompleted == numProcesses) {
            break;
        }

        // for each process with the 'current' time value, allocate to processor with shortest remaining time, or smallest id value
        if((processes[processTracker].timeArrived == clock) && (processTracker < numProcesses)) {
            while(1) {
                if(strcmp(&processes[processTracker].parallelisable, "n") == 0 || coreCount == 1) {
                    // allocate non-parallelisable process to the fastest processor, no splitting occurs
                    shortestId = 0;
                    shortestRemTime = processors[0].cpuRemainingTime;
                    for(int j=0; j<coreCount; j++) {
                        if(processors[j].cpuRemainingTime < shortestRemTime) {
                            shortestId = j;
                            shortestRemTime = processors[j].cpuRemainingTime;
                        }
                    }
                    enQueue(processors[shortestId].cpuQueue, processes[processTracker], &processors[shortestId].cpuRemainingTime, &processors[shortestId].back, &processors[shortestId].front);
                }
                else if(strcmp(&processes[processTracker].parallelisable, "p") == 0 && coreCount > 1) {
                    // for dual-cores, allocate to both. for 'N≥3 Cores', allocate according to x/k≥1 rule
                    if(coreCount == 2) { // split parallelisable process into 2 subprocesses
                        subTime = calculateSubTime(processes[processTracker].executionTime, 2); // calculate new execution time
                        processes[processTracker].executionTime = subTime; // update execution time in process table (processes[] array)
                        processes[processTracker].subProcessFin = 2;
                        enQueue(processors[0].cpuQueue, processes[processTracker], &processors[0].cpuRemainingTime, &processors[0].back, &processors[0].front);
                        enQueue(processors[1].cpuQueue, processes[processTracker], &processors[1].cpuRemainingTime, &processors[1].back, &processors[1].front);
                    } else if(coreCount > 2) { // split parallelisable process into N subprocesses
                        splitCount = calculateSplitCount(processes[processTracker].executionTime);
                        subTime = calculateSubTime(processes[processTracker].executionTime, splitCount);
                        processes[processTracker].executionTime = subTime; // update execution time in process table (processes[] array)
                        processes[processTracker].subProcessFin = splitCount;

                        // allocate parallelisable process that have just arrived to fastest processors
                        shortestId = 0;
                        shortestRemTime = processors[0].cpuRemainingTime;
                        for(int i=0; i<splitCount; i++) {
                            for(int j=0; j<coreCount; j++) {
                                if(processors[j].cpuRemainingTime <= shortestRemTime) {
                                    shortestId = i;
                                    shortestRemTime = processors[i].cpuRemainingTime;
                                }
                            }
                            enQueue(processors[shortestId].cpuQueue, processes[processTracker],  &processors[shortestId].cpuRemainingTime, &processors[shortestId].back, &processors[shortestId].front);
                        }
                    }
                }
                // the 'if' conditional below handles processes that arrive at the same clock time
                processTracker++;
                processesRemaining++;
                if((processes[processTracker].timeArrived != clock) || (processTracker >= numProcesses)) {
                    break;
                }
            }
        }

        // cycle through each processor, count how many will finish a process (NOT subprocess) in the current time step
        for(int k=0; k<coreCount; k++) {
            if(isFinishing(&processors[k], &processes) == 1) {
                numFin++;
                processesRemaining--;
            }
        }

        // run step function for each processor
        for(int k=0; k<coreCount; k++) {
            // step(&processors[k], &processesCompleted, &processes);
            if(cFlag == 0) {
                step(&processors[k], &processesCompleted, &processes);
            } else {
                challengeStep(&processors[k], &processesCompleted, &processes, quantum);
            }
        }
        // run loop for processors that have just finished a process, and have a non-empty waiting queue
        for(int k=0; k<coreCount; k++) {
            if(processors[k].state == 0) {
                // step(&processors[k], &processesCompleted, &processes);
                if(cFlag == 0) {
                    step(&processors[k], &processesCompleted, &processes);
                } else {
                    challengeStep(&processors[k], &processesCompleted, &processes, quantum);
                }
            }
        }
        clock++;
        numFin = 0;
    }

    // print performance statistics
    printf("Turnaround time %.0f\nTime overhead %.2f %.2f\nMakespan %d\n", ceil(turnaroundSummation/processesCompleted), maxTimeOverhead, overheadSummation/processesCompleted, (clock-1));

    // free mallocs
    for(int k=0; k<coreCount; k++) {
        free(processors[k].cpuQueue);
    }
    fclose(processesFile);
    processesFile = NULL;
    free(processes);

    return 0;
}