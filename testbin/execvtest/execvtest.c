#include <types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



int main() {
    const char *filename = "/testbin/add";  // Program path (const)
    char *args[4];                         // Writable array of pointers

    // Allocate writable copies of arguments
    args[0] = (char *)"add";               // Duplicate string for safety
    args[1] = (char *)("3");
    args[2] = (char *)("4");
    args[3] = NULL;

    pid_t pid = fork();
    if (pid == 0) {        
        printf("Exev test\n");             // Child process
        execv((const char *)filename, args);             // Pass filename and args
        // perror("execv failed");
        // exit(EXIT_FAILURE);
    }

    printf("Parent process continuing\n");
    return 0;
}
