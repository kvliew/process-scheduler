/*
CPU States: 0 idle, 1 running
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "scheduling.h"
#include "queue.h"

// each processor has its own step function, complete with its own process queue
void step(struct cpu *processor, int *processesCompleted) {
    if(processor->state == 0 && (processor->front != -1) && (processor->back != -1)) { // if the CPU is idle and its waiting queue is not empty
        processor->currentlyRunning = deQueue(processor);
        processor->cpuRemainingTime += processor->currentlyRunning.executionTime;
        printRunning(processor->currentlyRunning, processor->cpuId);
        processor->state = 1;
    } else if(processor->state == 1) { // CPU is running a process
        if(processor->cpuRemainingTime > 1) { // CPU is still running a process
            processor->cpuRemainingTime--;
        } else if(processor->cpuRemainingTime == 1) {
            printf("%d,FINISHED,id=%d,proc-remaining=%d\n", clock, processor->currentlyRunning.processId, *processesCompleted);
            processor->state = 0;
            (*processesCompleted)++;
        }
    }
}

void printRunning(struct process processEntry, int id) {
    printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", clock, currentlyRunning.processId, currentlyRunning.executionTime, id);
}


void checkShorterTime() {

}