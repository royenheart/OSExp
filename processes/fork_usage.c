/*  fork usage  */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    pid_t child;
    if ((child = fork()) == -1) { // 创建子进程，的
        // 返回 -1 表示创建子进程失败，此时调用 perror 函数返回错误信息
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (child == 0) {
        // 返回 0 表示当前上下文处于子进程中，执行子进程代码
        puts("in child");
        // 输出子进程 PID
        printf("\tchild pid = % d\n", getpid());
        // 输出父进程 PID
        printf("\tchild ppid = % d\n", getppid());
        exit(EXIT_SUCCESS);
    } else {
        // 当前处于父进程中，执行父进程代码
        puts("in parent");
        // 输出父进程 PID
        printf("\tparent pid = % d\n", getpid());
        // 输出父进程的父进程 PID
        printf("\tparent ppid = % d\n", getppid());
    }
    exit(EXIT_SUCCESS);
}