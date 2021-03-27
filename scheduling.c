#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "scheduling.h"
#include "queue.h"

// each processor has its own step function, complete with its own process queue
void step(struct cpu *processor, int *processesCompleted, struct process **processes) {
    // cpu running state
    if(processor->state == 1) {
        if(processor->cpuRemainingExec <= 1) { // CPU has just finished a process or subprocess
            if(processor->currentlyRunning.timeArrived != -1) {
                processor->cpuRemainingTime--;
                if(strcmp(&processor->currentlyRunning.parallelisable, "n") == 0) { // processor has finished a non-parallelisable process
                    (*processesCompleted)++;
                    printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, processesRemaining);
                    calculatePerformance(processor->currentlyRunning);
                } else { // processor has finished a subprocess process of a parallelisable process
                    if((*processes)[processor->currentlyRunning.processTableIndex].subProcessFin == 1 && (numFin > 0)) { // processor has finished all subprocesses of a process
                        (*processesCompleted)++;
                        (*processes)[processor->currentlyRunning.processTableIndex].subProcessFin = -1;
                        printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, processesRemaining);
                        calculatePerformance(processor->currentlyRunning);
                    }
                }
                processor->state = 0;
            }
            processor->currentlyRunning.timeArrived = -1; // once a process is completed, 'invalidate' it
        } else if(processor->cpuRemainingExec > 1) { // CPU is still running a process or subprocess
            if(processor->cpuQueue[processor->front].executionTime < processor->currentlyRunning.executionTime && (processor->front != -1) && (processor->back != -1)) {
                // SRTF Implementation
                processor->currentlyRunning.executionTime--;
                enQueue(processor->cpuQueue, processor->currentlyRunning, &processor->cpuRemainingTime, &processor->back, &processor->front);
                processor->currentlyRunning = deQueue(processor);
                processor->cpuRemainingExec = processor->currentlyRunning.executionTime;
                printRunning(processor->currentlyRunning, processor->cpuId);
            } else {
                // continue running process
                processor->currentlyRunning.executionTime--;
                processor->cpuRemainingExec--;
                processor->cpuRemainingTime--;
            }
        }
    }

    // cpu idle state
    else if(processor->state == 0 && (processor->front != -1) && (processor->back != -1)) {
        processor->currentlyRunning = deQueue(processor);
        processor->cpuRemainingExec = processor->currentlyRunning.executionTime;
        printRunning(processor->currentlyRunning, processor->cpuId);
        processor->state = 1;
    }
}

// the challenge task algorithm implements a version of the Round Robin Scheduling Algorithm
void challengeStep(struct cpu *processor, int *processesCompleted, struct process **processes, int quantum) {
    // cpu running state
    if(processor->state == 1) { // CPU is running a process or subprocess
        if(processor->cpuRemainingExec <= 1) { // CPU just finished a process, subprocess, or quantum
            if(processor->ending == 1) { // process/subprocess finished
                if(processor->currentlyRunning.timeArrived != -1) {
                    processor->cpuRemainingTime--;
                    if(strcmp(&processor->currentlyRunning.parallelisable, "n") == 0) { // processor has finished a non-parallelisable process
                        (*processesCompleted)++;
                        printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, processesRemaining); //numProcesses - (*processesCompleted)
                        calculatePerformance(processor->currentlyRunning);
                    } else {
                        if((*processes)[processor->currentlyRunning.processTableIndex].subProcessFin == 1 && (numFin > 0)) { // processor has finished all subprocesses of a process
                            (*processesCompleted)++;
                            (*processes)[processor->currentlyRunning.processTableIndex].subProcessFin = -1;
                            // processesRemaining--;
                            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, processesRemaining);
                            calculatePerformance(processor->currentlyRunning);
                        }
                    }
                    processor->state = 0;
                }
                processor->currentlyRunning.timeArrived = -1;

            } else if(processor->ending == 0) { // quantum finished
                enQueue(processor->cpuQueue, processor->currentlyRunning, &processor->cpuRemainingTime, &processor->back, &processor->front);
                processor->state = 0;
            }
        } else if(processor->cpuRemainingExec > 1) { // still running a process, continue
            processor->currentlyRunning.executionTime--;
            processor->cpuRemainingExec--;
            processor->cpuRemainingTime--;
        }
    }

    // cpu idle state
    else if(processor->state == 0 && (processor->front != -1) && (processor->back != -1)) {
        processor->currentlyRunning = deQueue(processor);
        processor->cpuRemainingExec = processor->currentlyRunning.executionTime;
        printRunning(processor->currentlyRunning, processor->cpuId);
        processor->state = 1;
        if(processor->currentlyRunning.executionTime > quantum) {
            processor->cpuRemainingExec = quantum;
            processor->ending = 0;
        } else if(processor->currentlyRunning.executionTime <= quantum) {
            processor->cpuRemainingExec = processor->currentlyRunning.executionTime + 1;
            processor->ending = 1;
        }
    }
}

// calculate how many times to split a parallelisable process for the N-Processor Scheduler (largest value of k such that x/k≥1)
int calculateSplitCount(int time) {
    int nSub = coreCount;
    int kMultiplier = 1; // k value such that such that x/k≥1
    if(cFlag == 1) {
        kMultiplier = 5;
    }

    for(int i=0; i<coreCount; i++) {
        if((kMultiplier*nSub) > time) {
            nSub--;
        }
    }
    if(nSub <= 1) {
        nSub = 1;
    }
    return nSub;
}

// calculate execution time of subprocess based on subprocess count
int calculateSubTime(float time, float numberOfSplits) {
    float temp = ceil((time/numberOfSplits)+1);
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

// returns 1 if the input processor will finish a process (NOT subprocess) in the current time step, 0 otherwise
// this function will also decrement the subProcessFin variable when it encounters a processor that is about to finish a non-final subprocess of a parallelisable process
int isFinishing(struct cpu *processor, struct process **processes) {
    int finishing = 0;
    if(processor->state == 1) { // CPU Running state
        if(processor->cpuRemainingExec <= 1) { // CPU has just finished a process or subprocess
            if(processor->currentlyRunning.timeArrived != -1) {
                if(strcmp(&processor->currentlyRunning.parallelisable, "n") == 0) { // non-para process finished
                    finishing = 1;
                } else { // processor has finished a subprocess of a process
                    if((*processes)[processor->currentlyRunning.processTableIndex].subProcessFin == 1) { // cpu finished final subprocess of a process
                        finishing = 1;
                    } else { // cpu finished non-final subprocess
                        (*processes)[processor->currentlyRunning.processTableIndex].subProcessFin--;
                    }
                }
            }
        }
    }
    return finishing;
}

// compare functions for qsort calls in main()
int cmpTimeArrivedExecTime(const void *a, const void *b) {
    struct process const *ia = a;
    struct process const *ib = b;

    if(ia->executionTime < ib->executionTime) {
        return -1;
    }
    if(ia->executionTime < ib->executionTime) {
        return 0;
    }

    return 1;
}

int cmpExecTimeProcessId(const void *a, const void *b) {
    struct process const *ia = a;
    struct process const *ib = b;

    if(ia->processId < ib->processId) {
        return -1;
    }
    if(ia->processId < ib->processId) {
        return 0;
    }

    return 1;
}