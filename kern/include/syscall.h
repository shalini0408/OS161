#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);

pid_t getpid();

pid_t getppid(); 
pid_t sys_fork(struct trapframe *tf);
int sys_execv(char *program, char **args); 
int sys_waitpid(pid_t pid, userptr_t status, int options);
int sys_exit(int code); 

void
md_forkentry(struct trapframe *tf);

#endif /* _SYSCALL_H_ */
