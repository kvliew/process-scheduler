#include <stdio.h>
#include <stdlib.h>

#include "scheduling.h"
#include "queue.h"

void enQueue(struct process cpuQueue[], struct process processEntry, int *cpuRemainingTime, int *back, int *front) {
    if(*back != (numProcesses-1)) {
        if(*front == -1) {
            *front = 0;
        }
        (*back)++;
        cpuQueue[*back] = processEntry;
        (*cpuRemainingTime) += processEntry.executionTime;

        // if no challenge task, sort by execution time then process id
        if(cFlag == 0) {
            // sort array by execution time
            qsort(cpuQueue, (*back)+1, sizeof(struct process), cmpTimeArrivedExecTime);
            // for processes with the same execution time, sort by process id
            int exec_dup = 1;
            int size = (*back) + 1;
            for(int k=0; k<size; k++) {
                if(cpuQueue[k].executionTime == cpuQueue[k+1].executionTime) {
                    exec_dup++;
                    if(k == size - 1) {
                        exec_dup--;
                    }
                }
                if(cpuQueue[k].executionTime != cpuQueue[k+1].executionTime || (k == (size - 1))) {
                    if(exec_dup > 1) {
                        qsort(&cpuQueue[k-exec_dup+1], exec_dup, sizeof(struct process), cmpExecTimeProcessId);
                        exec_dup = 1;
                    }
                }
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