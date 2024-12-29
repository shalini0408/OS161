/*
 * waitpidtest - does something. (You decide!)
 */

/* Include some standard headers (add more as needed) */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <thread.h>

int main() {
    // int waitReturn; 
    int status = -1; 

    
    pid_t f1 = fork();
    // printf("In test\n");
    // printf("In test Debug mode --1 :%d", pid);
    if (f1 < 0){
        printf("fork failed!\n");
        return 1;
    }
    //  f1 = 0; 
    if (f1 == 0) {        
        printf("child process with pid %d and parent pid %d\n", getpid(), getppid());
        exit(54); 
    }

    else{
        printf("parent process %d waiting for child %d\n", getpid(), f1); 
        // printf("f1 = %d\n", f1); 
        waitpid(f1, &status, 0);
        printf("parent %d finished waiting on child %d-- exit code of %d\n",getpid(), f1, status);
        exit(10);
    }

    return 0;



    // int wReturn1 =-1;
    // pid_t fork1PID; 
    // int wReturn2 = -1;
    // pid_t fork2PID; 
    // int status1; 
    // int status2; 

    // fork1PID = fork(); 
    // if (fork1PID == -1){
    //     printf("fork1error\n");
    // }
    // if (fork1PID == 0){ //if child 
    //     printf("fork 1 -- child %d\n", getpid());
    //     exit(1); 
    // }
    // else{
    //     printf("fork1 -- parent %d of child %d\n", getpid(), fork1PID);
    //     wReturn1 = waitpid(fork1PID, &status1, 0);
    //     printf("fork 1 -- parent %d finished waiting for child %d with exit status of %d\n", getpid(), wReturn1, status1);
    //     fork2PID = fork(); 
    //     if (fork2PID == 0){ //child 
    //         printf("fork 2 -- child %d\n", getpid());
    //         exit(2); 
    //     }
    //     else{
    //         printf("fork 2 -- parent %d of child %d\n", getpid(), fork2PID);
    //         wReturn2 = waitpid(fork2PID, &status2, 0); 
    //          printf("fork 2 -- parent %d finished waiting for child %d with exit status of %d\n", getpid(), wReturn2, status2);
    //         exit(30); 
    //     }

    // }

    // int wReturn1;
    // pid_t fork1PID; 
    // // int wReturn2;
    // // pid_t fork2PID; 
    // int status1; 
    // // int status2; 

    // fork1PID = fork(); 
    // if (fork1PID == 0){ //if child 
    //     // printf("fork 1 -- child %d\n", getpid());
    //     exit(1); 
    // }
    // else{
    //     // printf("fork1 -- parent %d of child %d\n", getpid(), fork1PID);
    //     waitpid(fork1PID, &status1, 0);
    //     // printf("fork 1 -- parent %d finished waiting for child %d with exit status of %d\n", getpid(), wReturn1, status1);
    //    exit (30); 
    // }
}
