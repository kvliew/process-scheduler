struct process {
    int timeArrived, processId, executionTime, originalExecutionTime;
    char parallelisable;
};

struct cpu {
    int cpuRemainingTime, cpuRemainingExec, front, back, state, cpuId;
    struct process currentlyRunning;
    struct process *cpuQueue;
};
/*
cpuRemainingTime: sum of the execution times of all processes in cpuQueue
cpuRemainingExec: time remaining for the current process the CPU is currently running
front: index 0 of cpuQueue
back: index of the last process in cpuQueue
state: value 0 idle, value 1 running
currentlyRunning: current process
cpuQueue
*/

int clock;
int processTracker;
int processesRemaining;
int numProcesses;
int coreCount; // number of processors specified in command line argument

// queue variables
int inputProcesses;

void step(struct cpu *processor, int *processesCompleted);
void printRunning(struct process processEntry, int id);
int calculateSplitCount();
int calculateSubTime(int time, int numberOfSplits);