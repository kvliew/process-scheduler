struct process {
    int timeArrived, processId, executionTime, originalExecutionTime, processTableIndex, subProcessFin;
    char parallelisable;
};

struct cpu {
    int cpuRemainingTime, cpuRemainingExec, front, back, state, cpuId, ending;
    struct process currentlyRunning;
    struct process *cpuQueue;
};
/*
Process Struct Fields
originalExecutionTime: this is a copy of executionTime and remains unchanged
throughout the entire program. This value is used for the overhead calculation
processTableIndex: stores the array index of the process within processes[]
subProcessFin: 'Sub-processes to Finish'; For a parallelisable process, this
field keeps track of the number of unfinished subprocesses of that process that
currently exist in the simulation.

CPU Struct Fields
cpuQueue: array of all unfinished processes and subprocesses assigned to CPU
cpuRemainingTime: sum of the execution times of all processes and sub processes
in cpuQueue
cpuRemainingExec: time remaining for the current process or subprocess the CPU
is currently running
front: index 0 of cpuQueue
back: index of the last process in cpuQueue
state: 0 for idle, 1 for running
currentlyRunning: the process or subprocess cpu is currently running
*/

int numFin; // stores the number of processes that will finish in the current time step
int clock;
int processesRemaining; // number of unfinished processes (NOT subprocesses) at any given time
int coreCount; // number of processors specified in command line argument
int numProcesses;
int cFlag; // 1 if the '-c' option is included in the command line argument, 1 otherwise

// performance statistics variables
float turnaroundSummation; // summation of ALL process turnaround times
float overheadSummation; // summation of ALL process overhead times
float maxTimeOverhead;
float avgTimeOverhead;
float avgTurnaroundTime;

void step(struct cpu *processor, int *processesCompleted, struct process **processes);
void challengeStep(struct cpu *processor, int *processesCompleted, struct process **processes, int quantum);
void printRunning(struct process processEntry, int id);
int calculateSplitCount(int time);
int calculateSubTime(float time, float numberOfSplits);
void calculatePerformance(struct process processEntry);
int isFinishing(struct cpu *processor, struct process **processes);
int cmpTimeArrivedExecTime(const void *a, const void *b);
int cmpExecTimeProcessId(const void *a, const void *b);