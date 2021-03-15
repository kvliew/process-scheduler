#include <stdio.h>
#include <stdlib.h>

#include "allocate.h"
#include "queue.h"

void enQueue() {
    if(back != (inputProcesses-1)) {
        if(front == -1) {
            front = 0;
        }
        back++;
        waitingQueue[back] = processEntry;
    }
}

struct process deQueue() {
    int difference;
    struct process temp;
    if(front == -1) {
        temp.timeArrived = -1;
        temp.processId = -1;
        temp.memorySizeReq = -1;
        temp.jobTime = -1;
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