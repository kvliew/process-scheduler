struct process {
    int timeArrived, processId, executionTime, cpuId, originalExecutionTime, timeRemaining;
    char parallelisable;
};
struct process currentlyRunning;

struct cpu {
    int cpuRemainingTime, front, back, state;
    struct process currentlyRunning;
    struct process *cpuQueue;
};

int clock;
int processTracker;
int processesRemaining;
int numProcesses;
int coreCount; // number of processors specified in command line argument

// queue variables
int inputProcesses;

void step(struct cpu *processor, int *processesCompleted);
void printRunning(struct process processEntry);