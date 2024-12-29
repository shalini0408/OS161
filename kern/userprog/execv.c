#include <types.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <machine/trapframe.h>
#include <kern/errno.h>
#include <addrspace.h>
#include <syscall.h>
#include <kern/limits.h>
#include <kern/unistd.h>
#include <vfs.h>
#include <string.h>
#define MAX_ARGS 128


int sys_execv(char *program, char **args){

    
    int openResult; 
    int elfResult; 
    int stackResult; 
    struct vnode *vnode; 
    vaddr_t entrypoint, stackptr;


    //if either argument is null, return EFAULT
    if (program == NULL || args == NULL){
        return EFAULT;
    }

    //alloate kernel space for program name 
    char *kernel_program = kmalloc(PATH_MAX); 
    if (kernel_program == NULL){
        return ENOMEM; 
    }

    //copy program name to kernel space 
    int pathcopied = copyinstr((const_userptr_t)program, kernel_program, PATH_MAX, NULL);
    if (pathcopied != 0){
        kprintf("Error in allocating spcae in kernel\n");
        kfree(kernel_program); 
        return pathcopied; 
    }
    kprintf("name = %s\n", kernel_program); 

    int argCounter = 0;
    userptr_t argsPtr = NULL;
    int copyResult = 0;
    int nullFound = 0;

// Allocate kernel buffer for the arguments
    char **kernelBuffer = kmalloc(MAX_ARGS * sizeof(char *));
    if (kernelBuffer == NULL) {
        kfree(kernel_program);
        return ENOMEM;
    }

// Step 1: Copy the argument pointers from user space to kernel space
    while (!nullFound) {
        copyResult = copyin((const_userptr_t)&args[argCounter], &argsPtr, sizeof(userptr_t));
        if (copyResult != 0) {
            kfree(kernel_program);
            kfree(kernelBuffer);
            return copyResult;
        }

        // Check if the argument pointer is NULL (end of arguments)
        if (argsPtr == NULL) {
            nullFound = 1;
        } 
        else{
            // Increment the argument counter and copy the argument string
            kernelBuffer[argCounter] = kmalloc(MAX_ARGS * sizeof(char));  // Allocate memory for the argument string
            if (kernelBuffer[argCounter] == NULL) {
                kfree(kernel_program);
                for (int j = 0; j <= argCounter; j++){
                    kfree(kernelBuffer[j]);
                }
                kfree(kernelBuffer);
                return ENOMEM;
            }

            // Copy the actual argument string from user space to kernel space
            copyResult = copyin((const_userptr_t)argsPtr, kernelBuffer[argCounter], MAX_ARGS * sizeof(char));
            if (copyResult != 0) {
                kfree(kernel_program);
                for (int j = 0; j <= argCounter; j++){
                    kfree(kernelBuffer[j]);
                }
                kfree(kernelBuffer);
                return copyResult;
            }
            int padding = 0;
            int padded_len = 0; 

             if (padding != 0) {
                memset(kernelBuffer[argCounter] + padded_len, 0, padding);
                kernelBuffer[argCounter][padded_len + padding] = '\0';  // Null-terminate
        }
        argCounter++;
        }
    }
   
   //add NULL to end 
    kernelBuffer[argCounter]= NULL;
    //Step 2: open executable and load elf into it 
    openResult = vfs_open(kernel_program, O_RDONLY, &vnode);
    if (openResult){
        kfree(kernel_program);
        kprintf("file opening FAILED -- %d\n", openResult);
        for (int j = 0; j <= argCounter; j++){
                    kfree(kernelBuffer[j]);
                }
        kfree(kernelBuffer);
        return openResult; 
    }


	/* Create a new address space. */
	curthread->t_vmspace = as_create();
	if (curthread->t_vmspace==NULL) {
		vfs_close(vnode);
		return ENOMEM;
	}

	/* Activate it. */
	as_activate(curthread->t_vmspace);

    /* Load the executable. */
	elfResult = load_elf(vnode, &entrypoint);
	if (elfResult) {
		vfs_close(vnode);
		return elfResult;
	}

	vfs_close(vnode);

    //Step 3: Copy arguments from kernel buffer into user stack

    /* Define the user stack in the address space */
	stackResult = as_define_stack(curthread->t_vmspace, &stackptr);
	if (stackResult) {
		return stackResult;
	}
    int outResult; 
    vaddr_t argv[argCounter+1];

    for (int m = 0; m < argCounter; m++){
        //decrease stackptr by 4 
        stackptr -= 4;
        outResult = copyout(kernelBuffer[m], (userptr_t)stackptr, 4);
        if (outResult != 0){
            kfree(kernel_program); //free memory 
            for (int j = 0; j <= argCounter; j++){
                        kfree(kernelBuffer[j]);
                    }
            kfree(kernelBuffer);
        return openResult; //return error
        }
        argv[m] = stackptr; 
    }

    argv[argCounter] = 0; 

    size_t sizeOfArgv = (argCounter + 1)*sizeof(vaddr_t); 
    int argvCopyResult; 
    stackptr -= sizeOfArgv; 


    //copy argv to stack 
    argvCopyResult = copyout(argv, (userptr_t)stackptr, sizeOfArgv); 
    if(argvCopyResult != 0){
        kprintf("argv copying to stack failed -- %d\n", argvCopyResult); 
        kfree(kernel_program); //free memory 
            for (int j = 0; j <= argCounter; j++){
                        kfree(kernelBuffer[j]);
                    }
            kfree(kernelBuffer);

    }

    //record address of argv array 
    vaddr_t argvAddr = stackptr; 


    //Step 4: return to usermode 
	md_usermode(argCounter /*argc*/, (userptr_t)argvAddr /*userspace addr of argv*/,
		    stackptr, entrypoint);
	
	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
    return 0; 
}


