// print file content
printf("Here is the file:\n");
c = fgetc(processesFile);
while(c != EOF) {
    printf("%c", c);
    c = fgetc(processesFile);
}
printf("\n");
printf("Number of processors: %d\n", numProcessors);

// print processes from array
for(int j=0; j<numProcesses; j++) {
    printf("Process %d: %d %d %d %c\n", j+1, processes[j].timeArrived, processes[j].processId, processes[j].executionTime, processes[j].parallelisable);
}

// test queue
enQueue(processors[0].cpuQueue, processes[0]);
enQueue(processors[0].cpuQueue, processes[1]);
printf("Here process 1: %d %d %d %c\n", processors[0].cpuQueue[0].timeArrived, processors[0].cpuQueue[0].processId, processors[0].cpuQueue[0].executionTime, processors[0].cpuQueue[0].parallelisable);
printf("Here process 2: %d %d %d %c\n", processors[0].cpuQueue[1].timeArrived, processors[0].cpuQueue[1].processId, processors[0].cpuQueue[1].executionTime, processors[0].cpuQueue[1].parallelisable);

// test queue 2
enQueue(&processors[0], processes[0]);
enQueue(&processors[0], processes[1]);
enQueue(&processors[0], processes[2]);
enQueue(&processors[0], processes[3]);
printf("Here is process 1: %d %d %d %c\n", processors[0].cpuQueue[0].timeArrived, processors[0].cpuQueue[0].processId, processors[0].cpuQueue[0].executionTime, processors[0].cpuQueue[0].parallelisable);
printf("Here is process 2: %d %d %d %c\n", processors[0].cpuQueue[1].timeArrived, processors[0].cpuQueue[1].processId, processors[0].cpuQueue[1].executionTime, processors[0].cpuQueue[1].parallelisable);
printf("Here is process 3: %d %d %d %c\n", processors[0].cpuQueue[2].timeArrived, processors[0].cpuQueue[2].processId, processors[0].cpuQueue[2].executionTime, processors[0].cpuQueue[2].parallelisable);
printf("Here is process 4: %d %d %d %c\n", processors[0].cpuQueue[3].timeArrived, processors[0].cpuQueue[3].processId, processors[0].cpuQueue[3].executionTime, processors[0].cpuQueue[3].parallelisable);