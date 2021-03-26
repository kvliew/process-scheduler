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
void step(struct cpu *processor, int *processesCompleted, struct process **processes) {
    // CPU RUNNING
    if(processor->state == 1) {
        if(processor->cpuRemainingExec <= 1) { // CPU has just finished a process or subprocess
            if(processor->currentlyRunning.timeArrived != -1) {
                processor->cpuRemainingTime--;
                if(strcmp(&processor->currentlyRunning.parallelisable, "n") == 0) {
                    // processor has finished a non-parallelisable process
                    (*processesCompleted)++;

                    // processesRemaining--;

                    printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, processesRemaining); //numProcesses - (*processesCompleted)
                    // printf("%d,FINISHED,pid=%d,proc_remaining=%d,cpu_id=%d\n", clock, processor->currentlyRunning.processId, processesRemaining, processor->cpuId); //numProcesses - (*processesCompleted)
                    // int temp = (processor->back) + 1;
                    //printf("Printing queue of CPU[%d] size %d\n", processor->cpuId, temp);
                    // for(int i=0; i<temp; i++) {
                    //     printf("%d %d %d %c\n", processor->cpuQueue[i].timeArrived, processor->cpuQueue[i].processId, processor->cpuQueue[i].executionTime, processor->cpuQueue[i].parallelisable);
                    // }
                    calculatePerformance(processor->currentlyRunning);
                } else {
                    // processor has finished a subprocess process of a parallelisable process and needs to perform further checks to determine if all subprocesses of a process has finished
                    if((*processes)[processor->currentlyRunning.subProcessIndex].subProcessFin == 1 && (numFin > 0)) {
                        // processor has finished all subprocesses of a process
                        (*processesCompleted)++;
                        (*processes)[processor->currentlyRunning.subProcessIndex].subProcessFin = -1;
                        // processesRemaining--;
                        // printf("%d,FINISHED LAST SUB,pid=%d,proc_remaining=%d,cpu_id=%d\n", clock, processor->currentlyRunning.processId, processesRemaining, processor->cpuId);
                        printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, processesRemaining);
                        calculatePerformance(processor->currentlyRunning);
                    } else {
                        // processor has finished a subprocess of a process, but more to be finished

                        // (*processes)[processor->currentlyRunning.subProcessIndex].subProcessFin--;

                        // printf("\t%d,CPU[%d] finished SUBPROCESS %d.%d, %d remaining\n", clock, processor->cpuId, processor->currentlyRunning.processId, processor->cpuId, (*processes)[processor->currentlyRunning.subProcessIndex].subProcessFin);
                        // printf("%d,CPU[%d]here1 %d\n", clock, processor->cpuId, (*processes)[processor->currentlyRunning.subProcessIndex].subProcessFin);
                    }
                }
                processor->state = 0;
            }

            processor->currentlyRunning.timeArrived = -1;
            // if((processor->front != -1) && (processor->back != -1)) { // if there are item(s) in the the waiting queue, start running the next one in the SAME time step
            //     step(processor, processesCompleted, processes);
            // }
        } else if(processor->cpuRemainingExec > 1) { // CPU is still running a process or subprocess
            if(processor->cpuQueue[processor->front].executionTime < processor->currentlyRunning.executionTime && (processor->front != -1) && (processor->back != -1) && (cFlag == 0)) {
                // if fastest process in cpu queue is faster than currently running process, interrupt the current process
                processor->currentlyRunning.executionTime--;
                // printf("%d,\t\t\tInterrupt CPU[%d] enqueuing %d %d %d %c\n", clock, processor->cpuId, processor->currentlyRunning.timeArrived, processor->currentlyRunning.processId, processor->currentlyRunning.executionTime, processor->currentlyRunning.parallelisable);
                enQueue(processor->cpuQueue, processor->currentlyRunning, &processor->cpuRemainingTime, &processor->back, &processor->front);
                processor->currentlyRunning = deQueue(processor);
                processor->cpuRemainingExec = processor->currentlyRunning.executionTime;
                printRunning(processor->currentlyRunning, processor->cpuId);
            } else {
                // continue running process
                processor->currentlyRunning.executionTime--;
                processor->cpuRemainingExec--;
                processor->cpuRemainingTime--;
                // printf("%d,CPU[%d] running process %d\tTotal remaining time %d, exec_time=%d\n", clock, processor->cpuId, processor->currentlyRunning.processId, processor->cpuRemainingTime, processor->cpuRemainingExec);
            }
        }
    }

    // CPU IDLE
    else if(processor->state == 0 && (processor->front != -1) && (processor->back != -1)) {
        // printf("%d,dequeuing CPU[%d]\n", clock, processor->cpuId);
        processor->currentlyRunning = deQueue(processor);
        //processor->cpuRemainingTime += processor->currentlyRunning.executionTime;
        processor->cpuRemainingExec = processor->currentlyRunning.executionTime;
        printRunning(processor->currentlyRunning, processor->cpuId);
        processor->state = 1;
    }
    else {
        // printf("%d,CPU[%d] queue is empty\n", clock, processor->cpuId);
    }
}

void challengeStep(struct cpu *processor, int *processesCompleted, struct process *processes, int quantum) {
    if(processor->state == 1) { // CPU is running a process or subprocess
        if(processor->cpuRemainingExec <= 1) { // just finished a process
            if(processor->ending == 1) { // process/subprocess finished
                if(processor->currentlyRunning.timeArrived != -1) {
                    processor->cpuRemainingTime--;
                    if(strcmp(&processor->currentlyRunning.parallelisable, "n") == 0) {
                        // processor has finished a non-parallelisable process
                        (*processesCompleted)++;
                        processesRemaining--;
                        printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, processesRemaining); //numProcesses - (*processesCompleted)
                        calculatePerformance(processor->currentlyRunning);
                    } else {
                        // processer has finished a parallelisable process and needs to perform further checks to determine if all subprocesses of a process has finished
                        if(processes[processor->currentlyRunning.subProcessIndex].subProcessFin == 1) {
                            // processor has finished all subprocesses of a process
                            (*processesCompleted)++;
                            processesRemaining--;
                            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", clock, processor->currentlyRunning.processId, processesRemaining);
                            calculatePerformance(processor->currentlyRunning);
                        } else {
                            // processor has finished a subprocess of a process, but more to be finished
                            processes[processor->currentlyRunning.subProcessIndex].subProcessFin--;
                        }
                    }
                    processor->state = 0;
                }
                processor->currentlyRunning.timeArrived = -1; // once a process is completed, 'invalidate' it
                if((processor->front != -1) && (processor->back != -1)) { // if there are item(s) in the the waiting queue, run the next one in the SAME time step
                    challengeStep(processor, processesCompleted, processes, quantum);
                }

            } else if(processor->ending == 0) { // quantum finished
                enQueue(processor->cpuQueue, processor->currentlyRunning, &processor->cpuRemainingTime, &processor->back, &processor->front);
                processor->state = 0;
                if((processor->front != -1) && (processor->back != -1)) { // if there are item(s) in the the waiting queue, start running the next one in the SAME time step
                    processor->currentlyRunning.timeArrived = -1;
                    challengeStep(processor, processesCompleted, processes, quantum);
                }
            }
        } else if(processor->cpuRemainingExec > 1) { // still running a process
            processor->currentlyRunning.executionTime--;
            processor->cpuRemainingExec--;
            processor->cpuRemainingTime--;
        }
    }

    else if(processor->state == 0 && (processor->front != -1) && (processor->back != -1)) {
        processor->currentlyRunning = deQueue(processor);
        //printf("\t\t\tHELLO1\n");
        //processor->cpuRemainingTime += processor->currentlyRunning.executionTime;
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
    int nSub = coreCount; // number of subprocesses
    int kMultiplier = 1;
    if(cFlag == 1) {
        kMultiplier = 4;
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

int isFinishing(struct cpu *processor, struct process **processes) {
    int finishing = 0;
    if(processor->state == 1) {
        if(processor->cpuRemainingExec <= 1) { // CPU has just finished a process or subprocess
            if(processor->currentlyRunning.timeArrived != -1) {
                if(strcmp(&processor->currentlyRunning.parallelisable, "n") == 0) {
                    // non-para
                    finishing = 1;
                } else {
                    // cpu finished subprocess
                    //printf("\t\t\t###DEBUG %d\n", processes[processor->currentlyRunning.subProcessIndex].subProcessFin);
                    if((*processes)[processor->currentlyRunning.subProcessIndex].subProcessFin == 1) { // cpu finished final subprocess
                        finishing = 1;
                    } else { // cpu finished non-final subprocess
                        (*processes)[processor->currentlyRunning.subProcessIndex].subProcessFin--;
                        // printf("%d,\t\tCPU[%d] curr_running_subprocessFin=%d exec_rem=%d\n", clock, processor->cpuId, (*processes)[processor->currentlyRunning.subProcessIndex].subProcessFin, processor->cpuRemainingExec);
                    }
                }
            }
        }
    }
    return finishing;
}

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