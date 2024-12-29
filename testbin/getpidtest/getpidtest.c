/*
 * getpidtest - does something. (You decide!)
 */

/* Include some standard headers (add more as needed) */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <err.h>

int
main() {
    /* Your code here... */
    
    printf("test pid = %d", getpid());

    return 0; 
}
