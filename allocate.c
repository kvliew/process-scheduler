#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "scheduling.h"

int main(int argc, char **argv) {
    // command line variables
    FILE *processesFile;
    int opt, numProcessors;
    char *fileName;

    // simulation variables
    clock = 0;
    int numProcesses;

    char c;
    struct process *processes;
    processes = NULL;

    while((opt = getopt(argc, argv, "f:p:c:")) != -1) {
        switch(opt) {
            case 'f':
                fileName = optarg;
                processesFile = fopen(fileName, "r");
                assert(processesFile != NULL);
                break;
            case 'p':
                numProcessors = atoi(optarg);
                break;
            case 'c': // challenge task
                break;
        }
    }

    // read content from processes file into array
    // extract process count from the processes file
    numProcesses = 1;
    for()for(c = getc(processesFile); c != EOF; c = getc(processesFile)) {
        if(c == '\n') {
            numProcesses++;
        }
    }
    // store processes into array
    rewind(processesFile);
    processes = (struct process *) malloc(numProcesses * sizeof(struct process));
    assert(processes);
    for(int i = 0; i < numProcesses; i++) {
        fscanf(inputFile, "%d %d %d %d", &processes[i].timeArrived, &processes[i].processId, &processes[i].executionTime, &processes[i].parallelisable);
        processes[i].originalExecutionTime = processes[i].executionTime; // a copy of the execution time is stored to calculate performance statistics
    }

    // initialise CPU waiting queue
    struct process waitingQueue[numProcesses];
    inputProcesses = numProcesses;
    processesRemaining = numProcesses;

    // simulation loop
    int processesCompleted = 0;
    while(1) {
        if(processesCompleted == numProcesses) {
            break;
        }
        if((processes[processTracker].timeArrived == clock) && (processTracker < numProcesses)) {
            // enqueue all processes with the 'current' time value
            while(1) {
                // add to queue enQueue(waitingQueue, processes[processTracker]);
                // processTracker++;
                if((processes[processTracker].timeArrived != clock) || (processTracker >= numProcesses)) {
                    break;
                }
            }
        }
        // step function fcfsStep(waitingQueue, &processesCompleted, memory);
        clock++;
    }

    // print performance statistics

    fclose(processesFile);
    processesFile = NULL;
    free(processes);

    return 0;
}

/* DEBUGGING
printf("Here is the file:\n");
c = fgetc(processesFile);
while(c != EOF) {
    printf("%c", c);
    c = fgetc(processesFile);
}
printf("\n");
printf("Number of processors: %d\n", numProcessors);
*/
