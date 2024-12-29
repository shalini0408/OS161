#include <types.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <lib.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <vm.h>
#include <vfs.h>
#include <test.h>

/*
 * Load program "progname" and start running it in usermode.
 * Supports command-line arguments.
 */
int runprogram(char *progname, char **args, int argc) {
    struct vnode *v;
    vaddr_t entrypoint, stackptr;
    int result;

    // Add null checks
    if (progname == NULL) {
        return EFAULT;
    }
    if (argc > 0 && args == NULL) {
        return EFAULT;
    }

    // Open the executable file
    result = vfs_open(progname, O_RDONLY, &v);
    if (result) {
        return result;
    }

    // Create a new address space
    assert(curthread->t_vmspace == NULL);
    curthread->t_vmspace = as_create();
    if (curthread->t_vmspace == NULL) {
        vfs_close(v);
        return ENOMEM;
    }

    // Activate the new address space
    as_activate(curthread->t_vmspace);

    // Load the ELF executable
    result = load_elf(v, &entrypoint);
    if (result) {
        vfs_close(v);
        return result;
    }

    // Close the executable file
    vfs_close(v);

    // Define the user stack
    result = as_define_stack(curthread->t_vmspace, &stackptr);
    if (result) {
        return result;
    }
    if(argc !=1){
    // Copy arguments from kernel space to user stack
    vaddr_t argv[argc + 1];
    for (int m = argc - 1; m >= 0; m--) {
        size_t len = strlen(args[m]) + 1;  // Include null terminator
        // Decrease stack pointer by actual string length
        stackptr -= len;
        // Copy the string to user stack
        int copyResult = copyoutstr(args[m], (userptr_t)stackptr, len, NULL);
        if (copyResult != 0) {
            return copyResult;
        }

        argv[m] = stackptr;
    }

    argv[argc] = 0;  // Null-terminate argv array

    // Align stack pointer
    stackptr = (stackptr & ~3);  // Align to 4 bytes

    // Copy argv array to user stack
    size_t argv_size = (argc + 1) * sizeof(vaddr_t);
    stackptr -= argv_size;
    int argvCopyResult = copyout(argv, (userptr_t)stackptr, argv_size);
    if (argvCopyResult != 0) {
        return argvCopyResult;
    }

    // Enter user mode
    md_usermode(argc, (userptr_t)stackptr, stackptr, entrypoint);
    }
    else{
        md_usermode(0 /*argc*/, NULL /*userspace addr of argv*/,
		    stackptr, entrypoint);
    }

    panic("md_usermode returned\n");
    return EINVAL;
}