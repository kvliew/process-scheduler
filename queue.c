#include <stdio.h>
#include <stdlib.h>

#include "scheduling.h"
#include "queue.h"

void enQueue(struct process cpuQueue[], struct process processEntry, int *cpuRemainingTime, int *back, int *front) { // sort array by execution time
    struct process temp;
    int sortingPointer;

    if(*back != (numProcesses-1)) {
        if(*front == -1) {
            *front = 0;
        }
        (*back)++;
        //printf("Enqueuing %d %d %d %c...\n", processEntry.timeArrived, processEntry.processId, processEntry.executionTime, processEntry.parallelisable);
        cpuQueue[*back] = processEntry;
        (*cpuRemainingTime) += processEntry.executionTime;
        //printf("Enqueued %d %d %d %c\n", cpuQueue[*back].timeArrived, cpuQueue[*back].processId, cpuQueue[*back].executionTime, cpuQueue[*back].parallelisable);

        // sort the array by execution time
        sortingPointer = *back;
        for(int i=0; i<(*back); i++) {
            if(cpuQueue[sortingPointer].executionTime <= cpuQueue[sortingPointer-1].executionTime) {
                temp = cpuQueue[sortingPointer-1];
                cpuQueue[sortingPointer-1] = cpuQueue[sortingPointer];
                cpuQueue[sortingPointer] = temp;
                sortingPointer--;
            } else {
                break;
            }
        }
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