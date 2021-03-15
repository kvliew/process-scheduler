#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char **argv) {
    // command line variables
    FILE *processesFile;
    int opt, numProcessors;

    char *fileName;

    char c;

    while((opt = getopt(argc, argv, "f:p:c:")) != -1) {
        switch(opt) {
            case 'f':
                fileName = optarg;
                processesFile = fopen(fileName, "r");
                assert(processesFile != NULL);
                break;
            case 'p':
                numProcessors = atoi(optarg);
                break;
            case 'c': // challenge task
                break;
        }
    }

    fclose(processesFile);

    /* DEBUGGING
    printf("Here is the file:\n");
    c = fgetc(processesFile);
    while(c != EOF) {
        printf("%c", c);
        c = fgetc(processesFile);
    }
    printf("\n");
    printf("Number of processors: %d\n", numProcessors);
    */

    return 0;
}
