#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);
pid_t getppid(void);

int global;

int main(int argc, char *argv[]) {
    int local = 0;
    int i;
    pid_t CurrentProcessID, ParentProcessID;
    CurrentProcessID = getpid();
    ParentProcessID = getppid();
    printf("Now it is in the program EXAMPLE4.\n");
    for (i = 0; i < 10; i++) {
        sleep(2);
        printf("Parent: %d, Current: %d, Number: %d\n", ParentProcessID,
               CurrentProcessID, i);
    }
    global = local + 1;
    exit(0);
}
