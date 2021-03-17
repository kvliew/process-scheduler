#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

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
    numProcesses = 1;
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

    // SIMULATION LOOP
    int processesCompleted = 0;
    int shortestId = 0; // id of CPU with smallest amount of remaining execution time
    int shortestRemTime = 9999;

    while(1) {
        if(processesCompleted == numProcesses) {
            break;
        }
        if((processes[processTracker].timeArrived == clock) && (processTracker < numProcesses)) {
            // for each process with the 'current' time value, allocate to processor with shortest remaining time, or smallest id value


            while(1) {
                // identify CPU with smallest remaining execution time
                //printf("before loop %d\n", shortestRemTime);
                for(int j=0; j<coreCount; j++) {
                    //printf("%d #DEBUG\tprocessId=%d processor[%d].cpuRemainingTime=%d\tshortestRemTime=%d\n", clock, processes[j].processId, j, processors[j].cpuRemainingTime, shortestRemTime);
                    if(processors[j].cpuRemainingTime < shortestRemTime) {
                        shortestId = j;
                        shortestRemTime = processors[j].cpuRemainingTime;
                    }
                    //printf("%d,       shortestRemTime=%d\n",clock, shortestRemTime);
                    //printf("%d,            Process:%d CPU:%d Time:%d\n", clock, processes[processTracker].processId, j, processors[j].cpuRemainingTime);
                }
                //printf("%d,\t\t\tSHORTEST ID IS %d\n", clock, shortestId);
                enQueue(processors[shortestId].cpuQueue, processes[processTracker], &processors[shortestId].cpuRemainingTime, &processors[shortestId].back, &processors[shortestId].front);
                processTracker++;
                shortestId = 0;
                shortestRemTime = 9999;
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