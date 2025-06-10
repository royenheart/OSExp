#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int pid;
    printf("one process now\n");
    printf("Parent calling fork()\n");
    pid = fork();
    if (pid == 0) {
        printf("child\n");
    } else if (pid > 0) {
        printf("parent\n");
    } else {
        printf("fork failed\n");
    }
    printf("end\n");
}
