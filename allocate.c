#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "scheduling.h"
#include "queue.h"

int main(int argc, char **argv) {
    // command line variables
    FILE *processesFile;
    char *fileName;
    int opt;

    // simulation variables
    clock = 0;

    char c;
    struct process *processes;
    processes = NULL;

    int quantum = -1;

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
            case 'c': // challenge task
                quantum = 60;
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
        processes[i].subProcessIndex = i;
        processes[i].subProcessFin = 9999;
    }

    // printf("Printing Original Process Table\n");
    // for(int i=0; i<numProcesses; i++) {
    //     printf("Process %d: %d %d %d %c\n", i, processes[i].timeArrived, processes[i].processId, processes[i].executionTime, processes[i].parallelisable);
    // }

    // sort processes array for processes that arrive at the same time by execution time
    int exec_dup = 1;
    //qsort(processes, numProcesses, sizeof(struct process), cmp_first);
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
                //printf("qsort %d %d\n", k-exec_dup+1, exec_dup);
                qsort(&processes[k-exec_dup+1], exec_dup, sizeof(struct process), cmp_first);
                exec_dup = 1;
            }
        }
    }
    // printf("\nPrinting Process Table sorted by exec_time\n");
    // for(int i=0; i<numProcesses; i++) {
    //     printf("Process %d: %d %d %d %c\n", i, processes[i].timeArrived, processes[i].processId, processes[i].executionTime, processes[i].parallelisable);
    // }

    exec_dup = 1;
    for(int k=0; k<numProcesses; k++) {
        if(processes[k].executionTime == processes[k+1].executionTime) {
            exec_dup++;
        } else {
            if(exec_dup > 1) {
                //printf("qsort %d %d\n", k-exec_dup+1, exec_dup);
                qsort(&processes[k-exec_dup+1], exec_dup, sizeof(struct process), cmp_second);
                exec_dup = 1;
            }
        }
    }
    // printf("\nPrinting Process Table sorted by id\n");
    // for(int i=0; i<numProcesses; i++) {
    //     printf("Process %d: %d %d %d %c\n", i, processes[i].timeArrived, processes[i].processId, processes[i].executionTime, processes[i].parallelisable);
    // }

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

    // SIMULATION LOOP
    int splitCount; // stores number of times a parallelisable process is split
    int subTime; // stores the execution time of a subprocess
    int processesCompleted = 0;
    int shortestId = 0; // id of CPU with smallest amount of remaining execution time (fastest processor)
    int shortestRemTime = 9999; // execution time of the fastest processor

    // Np Para
    int temp;
    int fastestId;


    while(1) {
        // all processes completed
        if(processesCompleted == numProcesses) {
            break;
        }

        // for each process with the 'current' time value, allocate to processor with shortest remaining time, or smallest id value
        if((processes[processTracker].timeArrived == clock) && (processTracker < numProcesses)) {
            while(1) {
                if(strcmp(&processes[processTracker].parallelisable, "n") == 0 || coreCount == 1) {
                    // allocate process to the fastest processor, no splitting occurs
                    for(int j=0; j<coreCount; j++) {
                        if(processors[j].cpuRemainingTime < shortestRemTime) {
                            shortestId = j;
                            shortestRemTime = processors[j].cpuRemainingTime;
                        }
                        //printf("%d,\t\tcpu[%d] Remaining Time %d\n", clock, j, processors[j].cpuRemainingTime);
                    }
                    //printf("%d,Enqueuing process %d to CPU %d\n", clock, processes[processTracker].processId, shortestId);
                    enQueue(processors[shortestId].cpuQueue, processes[processTracker], &processors[shortestId].cpuRemainingTime, &processors[shortestId].back, &processors[shortestId].front);
                    //printf("CPU %d remaining time %d\n\n", shortestId, processors[shortestId].cpuRemainingTime);
                    shortestId = 0;
                    shortestRemTime = 9999;
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

                        // enqueue processes to fastest processors
                        fastestId = 0;
                        temp = processors[0].cpuRemainingTime;
                        for(int i=0; i<splitCount; i++) {
                            for(int j=0; j<coreCount; j++) {
                                if(processors[j].cpuRemainingTime <= temp) {
                                    fastestId = i;
                                    temp = processors[i].cpuRemainingTime;
                                }
                            }
                            //printf("%d,Enqueuing %d %d %d %c into CPU %d\n", clock, processes[processTracker].timeArrived, processes[processTracker].processId, processes[processTracker].executionTime, processes[processTracker].parallelisable, fastestId);
                            enQueue(processors[fastestId].cpuQueue, processes[processTracker],  &processors[fastestId].cpuRemainingTime, &processors[fastestId].back, &processors[fastestId].front);
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
        // step function for each core
        /*
        for(int k=0; k<coreCount; k++) {
            step(&processors[k], &processesCompleted);
        }
        */

        // non-challenge loop
        if(quantum == -1) {
            // for(int k=0; k<coreCount; k++) {
            //     if(isFinishing(&processors[k], processes) == 1) {
            //         processesRemaining--;
            //     }
            // }

            // run step function for each processor
            for(int k=0; k<coreCount; k++) {
                //printf("\t%d,Running step function for CPU %d\n", clock, k);
                step(&processors[k], &processesCompleted, &processes);
            }
            // additional loop for processors that finished a process, but still have processes in their queues
            for(int k=0; k<coreCount; k++) {
                if(processors[k].state == 0) {
                    step(&processors[k], &processesCompleted, &processes);
                }
            }

            // FINISHING loop
            //printf("%d,\t\t\t#DEBUG# Starting FINISHED for-loop\n", clock);
            // for(int k=0; k<coreCount; k++) {
            //     if(isFinishing(&processors[k], processes) == 1) {
            //         //printf("\t%d,Running step function for CPU %d FINISHING\n", clock, k);
            //         step(&processors[k], &processesCompleted, &processes);
            //     }
            // }
            //
            // // RUNNING loop
            // //printf("%d,\t\t\t#DEBUG# Starting RUNNING for-loop\n", clock);
            // for(int k=0; k<coreCount; k++) {
            //     if(isFinishing(&processors[k], processes) == 0) {
            //         //printf("\t%d,Running step function for CPU %d RUNNING\n", clock, k);
            //         step(&processors[k], &processesCompleted, &processes);
            //     }
            // }
        } else {
            // challenge
            for(int k=0; k<coreCount; k++) {
                challengeStep(&processors[k], &processesCompleted, processes, quantum);
            }
        }
        clock++;
    }

    // print performance statistics here
    printf("Turnaround time %.0f\nTime overhead %.2f %.2f\nMakespan %d\n", ceil(turnaroundSummation/processesCompleted), maxTimeOverhead, overheadSummation/processesCompleted, (clock-1));

    for(int k=0; k<coreCount; k++) {
        free(processors[k].cpuQueue);
    }
    fclose(processesFile);
    processesFile = NULL;
    free(processes);

    return 0;
}