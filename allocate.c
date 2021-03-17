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
        processors[k].cpuQueue = malloc(sizeof(int)*numProcesses);
        processors[k].front = -1;
        processors[k].back = -1;
        processors[k].cpuRemainingTime = 0;
        processors[k].state = 0;
        processors[k].cpuRemainingExec = 0;
        processors[k].cpuId = k;
    }

    // SIMULATION LOOP
    int processesCompleted = 0;
    while(1) {
        if(processesCompleted == numProcesses) {
            break;
        }
        if((processes[processTracker].timeArrived == clock) && (processTracker < numProcesses)) {
            // for each process with the 'current' time value, allocate to processor with shortest remaining time, or smallest id value
            /* alternate loop
            for(int i=0; i<numProcesses; i++) {
                if(processes[i].timeArrived == clock) {
                    enQueue(processors[0].cpuQueue, processes[i], &processors[0].cpuRemainingTime, &processors[0].back, &processors[0].front); // add the process to a cpu queue; choice base on time remaining and/or id number
                    processTracker++;
                }
            }
            */
            while(1) {
                //printf("%d,Calling enqueue from main\n", clock);
                enQueue(processors[0].cpuQueue, processes[processTracker], &processors[0].cpuRemainingTime, &processors[0].back, &processors[0].front);
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