/*
 * getpidtest - does something. (You decide!)
 */

/* Include some standard headers (add more as needed) */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <err.h>

// int
// main() {
//     int pid;
//     pid = fork();
//     printf("fork test: %d\n", pid);
//     if (pid == 0) {
//     printf("Child process\n");
// } else {
//     printf("Parent process with child PID = %d\n", pid);
// }
//     return 0; 
// }

int main() {
  printf("A\n");
  fork();
  printf("B\n");
  if( fork() ) {
    printf("C\n");
  }
  else {
    printf("D\n");
  }
  return 0;
}
