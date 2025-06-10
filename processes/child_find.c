#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    int status;
    if (argc != 3) {
        printf("Usage: %s <path> <priority>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *path = argv[1];
    int pri = atoi(argv[2]);
    if (pri < PRIO_MIN || pri > PRIO_MAX) {
        printf("priority must be in range [%d, %d]\n", PRIO_MIN, PRIO_MAX);
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork failed!\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程
        // 打印当前优先级
        errno = 0;
        int me_pri = getpriority(PRIO_PROCESS, 0);
        if (me_pri == -1 && errno != 0) {
            perror("getpriority failed!");
            exit(EXIT_FAILURE);
        }
        printf("child priority = %d\n", me_pri);
        // 执行 find 指令
        execl("/usr/bin/find", "find", path, "-name", "hda*", (char *)NULL);
        perror("execl failed!");
        exit(EXIT_FAILURE);
    } else {
        // 父进程
        // 设置子进程优先级
        setpriority(PRIO_PROCESS, pid, pri);
        // 获取子进程结束状态
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) {
            printf("child exited with status: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("child pause due to signal, signal code is: %d\n",
                   WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("child pause because of pause signal code: %d\n",
                   WSTOPSIG(status));
        }
    }
}