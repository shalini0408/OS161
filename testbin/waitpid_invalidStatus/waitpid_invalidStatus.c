/*
 * waitpid_invalidStatus - does something. (You decide!)
 */

/* Include some standard headers (add more as needed) */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <thread.h>

int
main() {
    int fork1; 
    int wait;
    //int status; 

    fork1 = fork(); 

    if (fork1 == 0){
        printf("child\n"); 
        exit(132); 
    }

    else{
        //waitpid with invalid option 
        wait = waitpid(fork1, NULL, 0); 
        printf("wait = %d\n", wait);
    }
}
