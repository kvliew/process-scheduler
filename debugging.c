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