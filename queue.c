#include <stdio.h>
#include <stdlib.h>

#include "scheduling.h"
#include "queue.h"

void enQueue(struct cpu *processor, struct process processEntry) {
    if(processor->back != (numProcesses-1)) {
        if(processor->front == -1) {
            processor->front = 0;
        }
        processor->back++;
        printf("Enqueuing %d %d %d %c...\n", processEntry.timeArrived, processEntry.processId, processEntry.executionTime, processEntry.parallelisable);
        processor->cpuQueue[processor->back].timeArrived = processEntry.timeArrived;
        processor->cpuQueue[processor->back].processId = processEntry.processId;
        processor->cpuQueue[processor->back].executionTime = processEntry.executionTime;
        processor->cpuQueue[processor->back].parallelisable = processEntry.parallelisable;
        printf("Enqueued %d %d %d %c\n", processor->cpuQueue[processor->back].timeArrived, processor->cpuQueue[processor->back].processId, processor->cpuQueue[processor->back].executionTime, processor->cpuQueue[processor->back].parallelisable);
    }
}

struct process deQueue(struct cpu *processor) {
    int difference;
    struct process temp;
    if(processor->front == -1) {
        temp.timeArrived = -1;
        temp.processId = -1;
        temp.parallelisable = -1;
        temp.executionTime = -1;
        return temp;
    } else {
        difference = processor->back - processor->front;
        temp = processor->cpuQueue[processor->front];
        for(int i=0; i < difference; i++) {
            processor->cpuQueue[i] = processor->cpuQueue[i+1];
        }
        processor->back--;
        if(processor->front > processor->back) {
            processor->front = processor->back = -1;
        }
        return temp;
    }
}