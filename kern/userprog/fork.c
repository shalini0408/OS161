#include <types.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <machine/trapframe.h>
#include <kern/errno.h>
#include <addrspace.h>
#include <syscall.h>


pid_t sys_fork(struct trapframe *tf){
    struct thread *child;
    struct trapframe *child_tf;
    int result;

    child_tf = kmalloc(sizeof(struct trapframe));
    if (child_tf == NULL) {
        curthread->err = ENOMEM; 
        return -1; 
    }
    *child_tf = *tf;

    result = thread_fork("child", child_tf, 0, (void *)md_forkentry, &child);

    if (result) {
        kfree(child_tf);
        curthread->err = result; 
        return -1;
    }
    return child->pid;
}

