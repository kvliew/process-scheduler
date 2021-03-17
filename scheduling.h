struct process {
    int timeArrived, processId, executionTime, originalExecutionTime, timeRemaining;
    char parallelisable;
};
struct process currentlyRunning;

struct cpu {
    int cpuRemainingTime, front, back, state, cpuId;
    struct process currentlyRunning;
    struct process *cpuQueue;
};
/*
cpuRemainingTime: sum of the execution times of all processes in cpuQueue
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
void printRunning(struct process processEntry);