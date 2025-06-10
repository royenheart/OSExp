#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int pid;
    printf("1 process now\n");
    printf("parent call fork()\n");
    pid = fork();
    if (pid == 0) {
        printf("child\n");
        pid_t parentID = getppid();
        pid_t childID = getpid();
        printf("Parent ID: %d\nChild ID: %d\n", parentID, childID);
        execl("/bin/ls", "ls", "-l", "/", NULL);
        perror("exec error");
        exit(0);
    } else if (pid > 0) {
        pid_t who_return;
        who_return = wait(NULL);
        printf("child %d return, parent\n", who_return);
    } else {
        printf("fork failed\n");
    }
    printf("program end\n");
}
