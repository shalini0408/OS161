#include <types.h> 
#include <lib.h>
#include <curthread.h>
#include <thread.h>



pid_t getpid(){
    assert(curthread != NULL)
    return curthread->pid; 
}