#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

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
    processes = NULL;

    // store command line arguments
    while((opt = getopt(argc, argv, "f:p:c:")) != -1) {
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

    /*for(int i=0; i<numProcesses; i++) {
        printf("Process %d: %d %d %d %c\n", i, processes[i].timeArrived, processes[i].processId, processes[i].executionTime, processes[i].parallelisable);
    }*/

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
                    }
                    enQueue(processors[shortestId].cpuQueue, processes[processTracker], &processors[shortestId].cpuRemainingTime, &processors[shortestId].back, &processors[shortestId].front);
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
                        for(int j=0; j<coreCount; j++) {

                        }

                    }
                }
                // the 'if' conditional below handles processes that arrive at the same clock time
                processTracker++;
                if((processes[processTracker].timeArrived != clock) || (processTracker >= numProcesses)) {
                    break;
                }
            }
        }

        // step function for each core
        for(int k=0; k<coreCount; k++) {
            step(&processors[k], &processesCompleted);
        }
        clock++;
    }

    // print performance statistics here


    for(int k=0; k<coreCount; k++) {
        free(processors[k].cpuQueue);
    }
    fclose(processesFile);
    processesFile = NULL;
    free(processes);

    return 0;
}