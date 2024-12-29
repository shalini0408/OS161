/*
 * exittest - does something. (You decide!)
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
    
    pid_t pid; 

    pid = fork(); 

    if (pid < 0){
        printf("fork failed");
        return -1; 
    }

    if (pid ==0){ //is child 
        printf("child exiting with code 10\n");
        exit(10); 
    }

    printf("parent exiting with code 80\n"); 
    exit(80); 

    printf("all exited\n");
    return 0; 
}

