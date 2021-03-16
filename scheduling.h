struct process {
    int timeArrived, processId, executionTime, cpuId, originalExecutionTime;
    char parallelisable;
};
struct process currentlyRunning;

struct cpu {
    int cpuRemainingTime, front, back;
    struct process *cpuQueue;
};

int clock;
int state;
int processTracker;
int processesRemaining;
int numProcesses;
int coreCount; // number of processors specified in command line argument

// queue variables
int inputProcesses;

void step(struct process waitingQueue[], int *processesCompleted);
void printRunning(struct process processEntry);