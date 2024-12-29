#include <types.h> 
#include <lib.h>
#include <thread.h>
#include <curthread.h>



pid_t getppid(){

    struct thread* foundProcess; 
    pid_t new_ppid; 

    new_ppid = curthread->ppid; 

    kprintf("curthread -> ppid = %d\n", curthread->ppid);

    //find process in procTable if it exists 
    foundProcess = getProcess(new_ppid);

    //if not in table, return -1 
    if (foundProcess == NULL){
        return -1; 
    }

    //if parent has exited, return -1 
    if (foundProcess->is_zombie == 1){
        return -1; 
    }

    //if parent is the shell 
    if (new_ppid == 1){
        return -1; 
    }

    //otherwise return parent pid 
    return new_ppid;
}