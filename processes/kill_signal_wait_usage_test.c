/*  usage of kill,signal,wait  */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int flag;
int ww;

void stop();
void check_status(const char* who, int w, int status) {
    if (w == -1) {
        perror("waitpid");
        exit(-1);
    }
    if (WIFEXITED(status)) {
        printf("%s exited with status: %d\n", who, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("%s pause due to signal, signal code is: %d\n", who, WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
        printf("%s pause because of pause signal code: %d\n", who, WSTOPSIG(status));
    }
}

int main(int argc, char* argv[]) {
    int pid1, pid2;
    while ((pid1 = fork()) == -1);
    if ( pid1 > 0 ){
        while ((pid2 = fork()) == -1);
        if (pid2 > 0) {
            signal(2, stop);
            flag = 1;
            ww = getpid();
            int status1, status2;
            int w1, w2;
            sleep(1);
            kill(pid1, 16);
            kill(pid2, 17);
            w1 = waitpid(pid1, &status1, 0);
            w2 = waitpid(pid2, &status2, 0);
            printf("parent %d is killed when flag is %d\n", ww, flag);
            check_status("child1", w1, status1);
            check_status("child2", w2, status2);
            exit(flag);
        } else {
            flag = 1;
            ww = getpid();
            signal(17, stop);
            sleep(5);
            printf("child2 %d is killed by parent\n", ww);
            exit(flag);
        }
    } else {
        flag = 1;
        ww = getpid();
        signal(16, stop);
        sleep(5);
        printf("child1 %d is killed by parent\n", ww);
        exit(flag);
    }
}

void stop() {
    printf("pid %d: stop be called\n", ww);
    flag = 0;
}