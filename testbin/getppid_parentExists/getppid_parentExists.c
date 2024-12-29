/*
 * getppid_parentExists - does something. (You decide!)
 */

/* Include some standard headers (add more as needed) */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

int
main() {
    pid_t fork1; 

    fork1 = fork();
    int status;  

    if (fork1 == 0){ //if child 
    //get ppid of child 
        printf("child's parent pid = %d\n", getppid()); 

    }

    else{ //if parent 
        waitpid(fork1, &status, 0);
        //this should be the same as the ppid of the child above 
        printf("parent pid = %d", getpid()); 
    }

    return 0; 
    
}
