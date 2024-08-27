/*  usage of kill,signal,wait  */
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

int flag;
void stop();
int main(int argc, char *argv[]) {
    int pid1, pid2;
    signal(3, stop);
    while ((pid1 = fork()) == -1);
    if (pid1 > 0) {
        while ((pid2 = fork()) == -1);
        if (pid2 > 0) {
            flag = 1;
            sleep(5);
            kill(pid1, 16);
            kill(pid2, 17);
            wait(0);
            wait(0);
            printf("\n parent is killed\n");
            exit(EXIT_SUCCESS);
        } else {
            flag = 1;
            signal(17, stop);
            printf("\n child2 is killed by parent\n");
            exit(EXIT_SUCCESS);
        }
    } else {
        flag = 1;
        signal(16, stop);
        printf("\n child1 is killed by parent\n");
        exit(EXIT_SUCCESS);
    }
}

void stop() {
    flag = 0;
}