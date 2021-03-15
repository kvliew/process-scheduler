#include <stdio.h>
#include <stdlib.h>

#include "scheduling.h"
#include "queue.h"

void enQueue(struct process waitingQueue[], struct process processEntry) {
    if(back != (inputProcesses-1)) {
        if(front == -1) {
            front = 0;
        }
        back++;
        waitingQueue[back] = processEntry;
    }
}

struct process deQueue(struct process waitingQueue[]) {
    int difference;
    struct process temp;
    if(front == -1) {
        temp.timeArrived = -1;
        temp.processId = -1;
        temp.parallelisable = -1;
        temp.executionTime = -1;
        return temp;
    } else {
        difference = back - front;
        temp = waitingQueue[front];
        for(int i=0; i < difference; i++) {
            waitingQueue[i] = waitingQueue[i+1];
        }
        back--;
        if(front > back) {
            front = back = -1;
        }
        return temp;
    }
}