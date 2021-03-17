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
    if(processor->state == 0 && (processor->front != -1) && (processor->back != -1)) { // CPU is idle and its waiting queue is not empty
        processor->currentlyRunning = deQueue(processor);
        processor->cpuRemainingTime += processor->currentlyRunning.executionTime;
        processor->cpuRemainingExec = processor->currentlyRunning.executionTime;
        printRunning(processor->currentlyRunning, processor->cpuId);
        processor->state = 1;
    } else if(processor->state == 1) { // CPU is running a process
        if(processor->cpuRemainingExec > 1) { // CPU is still running a process
            if(processor->cpuQueue[processor->front].executionTime < processor->currentlyRunning.executionTime && (processor->front != -1) && (processor->back != -1)) {
                enQueue(processor->cpuQueue, processor->currentlyRunning, &processor->cpuRemainingTime, &processor->back, &processor->front);
                processor->currentlyRunning = deQueue(processor);
                processor->cpuRemainingExec = processor->currentlyRunning.executionTime;
                printRunning(processor->currentlyRunning, processor->cpuId);
            }
            processor->currentlyRunning.executionTime--;
            processor->cpuRemainingExec--;
            processor->cpuRemainingTime--;
        } else if(processor->cpuRemainingExec == 1) { // CPU has finished a process
            if(processor->currentlyRunning.timeArrived != -1) {
                (*processesCompleted)++;
                printf("%d,FINISHED,id=%d,proc-remaining=%d\n", clock, processor->currentlyRunning.processId, numProcesses - (*processesCompleted));
                processor->state = 0;
            }
            processor->currentlyRunning.timeArrived = -1;
            if((processor->front != -1) && (processor->back != -1)) { // if there are item(s) in the the waiting queue, start running the next one in the SAME time step
                step(processor, processesCompleted);
            }
        }
    }
}

void printRunning(struct process processEntry, int id) {
    printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", clock, processEntry.processId, processEntry.executionTime, id);
}