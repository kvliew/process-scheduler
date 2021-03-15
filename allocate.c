#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv) {
    // command line variables
    FILE *processesFile;
    int opt, numProcessors;

    char *fileName;

    while((opt = getopt(argc, argv, "f:p:c:")) != -1) {
        switch(opt) {
            case 'f':
                fileName = optarg;
                processesFile = fopen(fileName, 'r');
                assert(processesFile != NULL);
                break;
            case 'p':
                numProcessors = atoi(optarg);
                break;
            case 'c': // challenge task
                break;
        }
    }

    return 0;
}
