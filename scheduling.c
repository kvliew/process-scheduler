/*
CPU States: 0 idle, 1 running
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "scheduling.h"
#include "queue.h"

// each processor has its own step function, complete with its own process queue
void step(struct cpu *processor, int *processesCompleted) {
    if(processor->state == 1) {
        // CPU is running
        if(processor->cpuRemainingExec <= 1) { // CPU has just finished a process or subprocess
            if(processor->currentlyRunning.timeArrived != -1) {
                if(strcmp(&processor->currentlyRunning.parallelisable, "n") == 0) {
                    // processor has finished a non-parallelisable process
                    (*processesCompleted)++;
                    printf("%d,FINISHED,id=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, numProcesses - (*processesCompleted));
                    calculatePerformance(processor->currentlyRunning);
                } else {
                    // processer has finished a parallelisable process and needs to perform further checks to determine if all subprocesses of a process has finished
                    if(processes[processor->currentlyRunning.subProcessIndex].subProcessFin == 1) {
                        // processor has finished all subprocesses of a process
                        (*processesCompleted)++;
                        printf("%d,FINISHED,id=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, numProcesses - (*processesCompleted));
                        calculatePerformance(processor->currentlyRunning);
                    } else {
                        // processor has finished a subprocess of a process, but more to be finished
                        processes[processor->currentlyRunning.subProcessIndex].subProcessFin--;
                    }
                }
                processor->state = 0;
            }
            processor->currentlyRunning.timeArrived = -1;
            if((processor->front != -1) && (processor->back != -1)) { // if there are item(s) in the the waiting queue, start running the next one in the SAME time step
                step(processor, processesCompleted);
            }
        } else if(processor->cpuRemainingExec > 1) { // CPU is still running a process or subprocess
            if(processor->cpuQueue[processor->front].executionTime < processor->currentlyRunning.executionTime && (processor->front != -1) && (processor->back != -1)) {
                enQueue(processor->cpuQueue, processor->currentlyRunning, &processor->cpuRemainingTime, &processor->back, &processor->front);
                processor->currentlyRunning = deQueue(processor);
                processor->cpuRemainingExec = processor->currentlyRunning.executionTime;
                printRunning(processor->currentlyRunning, processor->cpuId);
            }
            processor->currentlyRunning.executionTime--;
            processor->cpuRemainingExec--;
            processor->cpuRemainingTime--;
        }
    }

    else if(processor->state == 0 && (processor->front != -1) && (processor->back != -1)) {
        // CPU is idle and its waiting queue is not empty
        processor->currentlyRunning = deQueue(processor);
        processor->cpuRemainingTime += processor->currentlyRunning.executionTime;
        processor->cpuRemainingExec = processor->currentlyRunning.executionTime;
        printRunning(processor->currentlyRunning, processor->cpuId);
        processor->state = 1;
    }
}

// calculate how many times to split a parallelisable process for the N-Processor Scheduler (largest value of k such that x/k≥1)
int calculateSplitCount(int time) {
    int nSub = coreCount; // number of subprocesses
    for(int i=0; i<coreCount; i++) {
        if(nSub > time) {
            nSub--;
        }
    }
    return nSub;
}

// calculate execution time of subprocess based on subprocess count
int calculateSubTime(float time, float numberOfSplits) {
    float temp = ceil((time/numberOfSplits)+1);
    //printf("%d,\t\t\t#DEBUG#\tTime=%f\tSplits=%f\n", clock, time, numberOfSplits);
    //printf("%d,\t\t\t#DEBUG#\t%f\n", clock, ceil(time/numberOfSplits));
    return temp;
}

void printRunning(struct process processEntry, int id) {
    if(strcmp(&processEntry.parallelisable, "n") == 0) {
        printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", clock, processEntry.processId, processEntry.executionTime, id);
    } else {
        printf("%d,RUNNING,pid=%d.%d,remaining_time=%d,cpu=%d\n", clock, processEntry.processId, id, processEntry.executionTime, id);
    }
}

void calculatePerformance(struct process processEntry) {
    float newTurnaroundTime = clock - processEntry.timeArrived;
    float newTimeOverhead = newTurnaroundTime/processEntry.originalExecutionTime;

    // Turnaround calculation
    turnaroundSummation = turnaroundSummation + newTurnaroundTime;

    // Overhead calculation
    overheadSummation = overheadSummation + newTimeOverhead;
    if(newTimeOverhead > maxTimeOverhead) {
        maxTimeOverhead = newTimeOverhead;
    }
}