struct process {
    int timeArrived, processId, executionTime, originalExecutionTime;
    char parallelisable;
};
struct process currentlyRunning;

int clock;
int state;
int processTracker;
int timeRemaining;
int processesRemaining;

int coreCount; // number of CPUs specified in command line argument

// queue variables
int inputProcesses;

void printRunning(struct process processEntry);