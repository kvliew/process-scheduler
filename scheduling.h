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

int numFin;
int clock;
int processesRemaining;
int coreCount; // number of processors specified in command line argument
int numProcesses;

// global variables removed
// int processTracker;

//struct process *processes; // process table

// performance statistics variables
float turnaroundSummation; // summation of ALL process turnaround times
float overheadSummation; // summation of ALL process overhead times
float maxTimeOverhead;

float avgTimeOverhead;
float avgTurnaroundTime;

// queue variables
// int inputProcesses;

void step(struct cpu *processor, int *processesCompleted, struct process **processes);
void challengeStep(struct cpu *processor, int *processesCompleted, struct process *processes, int quantum);
void printRunning(struct process processEntry, int id);
int calculateSplitCount(int time);
int calculateSubTime(float time, float numberOfSplits);
void calculatePerformance(struct process processEntry);
int isFinishing(struct cpu *processor, struct process **processes);
// int cmp_exec(const void *a, const void *b);
int cmpTimeArrivedExecTime(const void *a, const void *b);
int cmpExecTimeProcessId(const void *a, const void *b);