/*
CPU States: 0 idle, 1 running
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "scheduling.h"
#include "queue.h"

void step(struct process waitingQueue[], int *processesCompleted) {

}

void printRunning(struct process processEntry) {
    if(coreCount == 1) {
        printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", clock, currentlyRunning.processId, currentlyRunning.executionTime, currentlyRunning.cpuId);
    } else {

    }
}