struct process {
    int timeArrived, processId, executionTime, parallelisable, cpuId, originalExecutionTime;
};
struct process currentlyRunning;

int clock;
int state;
int processTracker;
int processesRemaining;
int coreCount; // number of CPUs specified in command line argument

// queue variables
int inputProcesses;

void step(struct process waitingQueue[], int *processesCompleted);
void printRunning(struct process processEntry);