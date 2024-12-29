/*
 * getppid_parentDoesNotExist - does something. (You decide!)
 */

/* Include some standard headers (add more as needed) */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

int
main() {

    int result; 

    result = getppid(); 
    printf("getppid() = %d\n", result); //should return -1 because only shell is the parent 

  

    return 0; 
    
}
