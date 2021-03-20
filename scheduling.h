struct process {
    int timeArrived, processId, executionTime, originalExecutionTime, subProcessIndex, subProcessFin;
    char parallelisable;
};

struct cpu {
    int cpuRemainingTime, cpuRemainingExec, front, back, state, cpuId, ending;
    struct process currentlyRunning;
    struct process *cpuQueue;
};
/*
cpuRemainingTime: sum of the execution times of all processes and sub processes in cpuQueue
cpuRemainingExec: time remaining for the current process or subprocess the CPU is currently running
front: index 0 of cpuQueue
back: index of the last process in cpuQueue
state: 0 for idle, 1 for running
currentlyRunning: process or subprocess cpu is currently running
cpuQueue: array of all unfinished processes and subprocesses assigned to CPU
*/

int clock;
int processTracker;
int processesRemaining;
int numProcesses;
int coreCount; // number of processors specified in command line argument

struct process *processes; // process table

// performance statistics variables
float turnaroundSummation; // summation of ALL process turnaround times
float overheadSummation; // summation of ALL process overhead times
float maxTimeOverhead;

float avgTimeOverhead;
float avgTurnaroundTime;

// queue variables
int inputProcesses;

void step(struct cpu *processor, int *processesCompleted);
void challengeStep(struct cpu *processor, int *processesCompleted, int quantum);
void printRunning(struct process processEntry, int id);
int calculateSplitCount(int time);
int calculateSubTime(float time, float numberOfSplits);
void calculatePerformance(struct process processEntry);