#include <stdio.h>
#include <stdlib.h>

#include "scheduling.h"
#include "queue.h"

void enQueue(struct process cpuQueue[], struct process processEntry, int *cpuRemainingTime, int *back, int *front) { // sort array by execution time
    // struct process temp;
    // int sortingPointer;

    if(*back != (numProcesses-1)) {
        if(*front == -1) {
            *front = 0;
        }
        (*back)++;
        //printf("%d,Enqueuing %d %d %d %c...\n", clock, processEntry.timeArrived, processEntry.processId, processEntry.executionTime, processEntry.parallelisable);
        cpuQueue[*back] = processEntry;
        (*cpuRemainingTime) += processEntry.executionTime;
        //printf("%d,Enqueued %d %d %d %c\n", clock, cpuQueue[*back].timeArrived, cpuQueue[*back].processId, cpuQueue[*back].executionTime, cpuQueue[*back].parallelisable);

        // sort queue by execution time
        // sortingPointer = *back;
        // for(int i=0; i<(*back); i++) {
        //     if(cpuQueue[sortingPointer].executionTime <= cpuQueue[sortingPointer-1].executionTime) {
        //         temp = cpuQueue[sortingPointer-1];
        //         cpuQueue[sortingPointer-1] = cpuQueue[sortingPointer];
        //         cpuQueue[sortingPointer] = temp;
        //         sortingPointer--;
        //     } else {
        //         break;
        //     }
        // }

        // using qsort()
        qsort(cpuQueue, (*back)+1, sizeof(struct process), cmp_first);

        // printf("\nPrinting Unsorted Queue\n");
        // for(int i=0; i<numProcesses; i++) {
        //     printf("Process %d: %d %d %d %c\n", i, cpuQueue[i].timeArrived, cpuQueue[i].processId, cpuQueue[i].executionTime, cpuQueue[i].parallelisable);
        // }

        // for processes with the same execution time, sort by process id
        int exec_dup = 1;
        int size = (*back) + 1;
        for(int k=0; k<size; k++) {
            if(cpuQueue[k].executionTime == cpuQueue[k+1].executionTime) {
                exec_dup++;
                //printf("k %d exec_dup %d\n", k, exec_dup);
                if(k == size - 1) {
                    exec_dup--;
                }
            }
            if(cpuQueue[k].executionTime != cpuQueue[k+1].executionTime || (k == (size - 1))) {
                if(exec_dup > 1) {
                    //printf("qsort %d %d\n", k-exec_dup+1, exec_dup);
                    qsort(&cpuQueue[k-exec_dup+1], exec_dup, sizeof(struct process), cmp_second);
                    exec_dup = 1;
                }
            }
        }
        // printf("\nPrinting Queue sorted by id\n");
        // for(int i=0; i<numProcesses; i++) {
        //     printf("Process %d: %d %d %d %c\n", i, cpuQueue[i].timeArrived, cpuQueue[i].processId, cpuQueue[i].executionTime, cpuQueue[i].parallelisable);
        // }
        //
        // print queue
        // printf("\t\t\tPRINTING QUEUE...\n");
        // for(int i=0; i<numProcesses; i++) {
        //     printf("%d,\t\t\t###DEBUG###\t %d %d %d %c\n", clock, cpuQueue[i].timeArrived, cpuQueue[i].processId, cpuQueue[i].executionTime, cpuQueue[i].parallelisable);
        // }
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