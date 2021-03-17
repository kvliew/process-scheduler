int front, back;
void enQueue(struct process cpuQueue[], struct process processEntry, int *cpuRemainingTime, int *back, int *front);
struct process deQueue(struct cpu *processor);