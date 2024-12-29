#include <types.h> 
#include <lib.h>
#include <curthread.h>
#include <thread.h>
#include <kern/errno.h>
#include <synch.h>
#include <machine/spl.h>



int sys_exit(int code){

    int spl = splhigh();
    lock_acquire(procTableLock); 

    //set exit status 
    curthread->exit_status = code; 
  
    // kprintf("thread %d exiting with code of %d\n", curthread->pid, curthread->exit_status);

    lock_release(procTableLock); 
    splx(spl); 
    //exit thread
    thread_exit(); 
    return code; //this line should never be reached 
}