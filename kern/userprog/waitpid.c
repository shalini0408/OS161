#include <types.h> 
#include <lib.h>
#include <curthread.h>
#include <thread.h>
#include <kern/errno.h>
#include <synch.h>
#include <machine/spl.h>

int sys_waitpid(pid_t pid, userptr_t status, int options) {
    int ptrCheck; 
    int statusCheck; 

    lock_acquire(procTableLock); 
    assert (pid > 0 && pid < 128); 

    //if options is not valid, return -1 and set error to EINVAL
    if (options != 0) {
        
        curthread->err = EINVAL; 
        return -1; 
    }

    // Validate status pointer
    if (status == NULL) {
        curthread->err = EFAULT; 
        return -1; 
    }
    
    //if status can be successfully copied in, then its pointer if valid 
    ptrCheck = copyin(status, &statusCheck, sizeof(int));
    if (ptrCheck != 0){
        curthread->err = ptrCheck;
        return -1; 
    }

    lock_release(procTableLock); 
    struct thread *childProc = getProcess(pid); 
    lock_acquire(procTableLock); 
    
    
    // kprintf("child exit status = %d\n", curthread->child_exit_status);
    //if child proc with this pid does not exist, child has already finished, return child_exit_status of current thread
    if (childProc == NULL && curthread->child_exit_status != -1){
        int result = copyout(&curthread->child_exit_status, status, sizeof(int));
        if (result != 0) {
            curthread->err = EFAULT;
            lock_release(procTableLock); 
            return -1;
        }
        lock_release(procTableLock); 
        return (curthread->child_pid); 
    }

    //otherwise pid is not valid 
    else if (childProc == NULL && curthread->child_exit_status == -1){
        kprintf("invalid child: pid = %d, exit status = %d\n", curthread->child_pid, curthread->child_exit_status);
        lock_release(procTableLock); 
        curthread->err = EINVAL;
        return -1; 
    }

    //make sure childProc is not null 
    assert(childProc != NULL);
   
    int spl = splhigh(); 
    childProc->waitingParent = curthread;

    lock_release(procTableLock);
    //Sleep parent
    thread_sleep(curthread); 
    lock_acquire(procTableLock); 

    //after waking, copy out exit status 
    int result = copyout(&curthread->child_exit_status, status, sizeof(int));

    if (result != 0) {
        curthread->err = EFAULT; 
        lock_release(procTableLock); 
        splx(spl);
        return -1; 
    }
    
    lock_release(procTableLock);
    splx(spl);
    
    return curthread->child_pid;
}

