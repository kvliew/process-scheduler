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
    front = -1;
    back = -1;

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

    // extract process count from the processes file
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
        processes[i].cpuId = -1; // not yet assigned a processor
    }

    // initialise array of CPUs
    struct cpu processors[coreCount];
    for(int k=0; k<coreCount; k++) {
        processors[k].cpuQueue = malloc(sizeof(int)*numProcesses);
    }

    enQueue(processors[0].cpuQueue, processes[0]);
    printf("Here is the process: %d %d %d %c\n", processors[0].cpuQueue[0].timeArrived, processors[0].cpuQueue[0].processId, processors[0].cpuQueue[0].executionTime, processors[0].cpuQueue[0].parallelisable);

    /*
    // simulation loop
    int processesCompleted = 0;
    while(1) {
        if(processesCompleted == numProcesses) {
            break;
        }
        if((processes[processTracker].timeArrived == clock) && (processTracker < numProcesses)) {
            // enqueue all processes with the 'current' time value
            while(1) {
                enQueue(processors[0].cpuQueue, processes[processTracker]);
                processTracker++;
                if((processes[processTracker].timeArrived != clock) || (processTracker >= numProcesses)) {
                    break;
                }
            }
        }
        step(waitingQueue, &processesCompleted);
        clock++;
    }
    */

    // print performance statistics here

    // DEBUGGING
    for(int j=0; j<numProcesses; j++) {
        printf("Process %d: %d %d %d %c\n", j+1, processes[j].timeArrived, processes[j].processId, processes[j].executionTime, processes[j].parallelisable);
    }

    for(int k=0; k<coreCount; k++) {
        free(processors[k].cpuQueue);
    }
    fclose(processesFile);
    processesFile = NULL;
    free(processes);

    return 0;
}