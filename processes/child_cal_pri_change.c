#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#ifdef STRESS
#include "stress_test.h"
#endif

#define N 1000

// 声明时间打印输出的函数
void time_print(char *, clock_t);

int main(int argc, char *argv[]) {
    pid_t pid;
    int status;
    clock_t start, end;
    struct tms t_start, t_end;

    if (argc != 2) {
        printf("Usage: %s <priority>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int pri = atoi(argv[1]);
    if (pri < PRIO_MIN || pri > PRIO_MAX) {
        printf("priority must be in range [%d, %d]\n", PRIO_MIN, PRIO_MAX);
        exit(EXIT_FAILURE);
    }

    start = times(&t_start);

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
#ifndef STRESS
        char arr[N][N] = {0};
        int k = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                arr[i][j] = ++k;
                if (k > 126) {
                    k = 1;
                }
            }
        }
#else
        // 使用计算密集型代码
        matrix_cal(10000, 8000, 8000);
#endif
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
            end = times(&t_end);
            printf("child exited with status: %d\n", WEXITSTATUS(status));
            // 打印进程计算经历的时间（真实时间）
            time_print("elapsed", end - start);
            // 打印父进程各个模式下使用的 CPU 时间
            puts("parent times");
            time_print("\tuser CPU", t_end.tms_utime);
            time_print("\tsys CPU", t_end.tms_stime);
            // 打印子进程各个模式下使用的 CPU 时间
            puts("child times");
            time_print("\tuser CPU", t_end.tms_cutime);
            time_print("\tsys CPU", t_end.tms_cstime);
        } else if (WIFSIGNALED(status)) {
            printf("child pause due to signal, signal code is: %d\n",
                   WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("child pause because of pause signal code: %d\n",
                   WSTOPSIG(status));
        }
    }
}

void time_print(char *str, clock_t time) {
    // 获取系统每秒所包含的 CPU
    // 时钟数（时钟滴答数，即一秒内有多少个时钟周期），用 sysconf
    // 函数获取符合实际。
    long tps = sysconf(_SC_CLK_TCK);
    // 根据每秒时钟数和运行时钟数算出精确经历的秒数
    printf("%s: %6.2f secs\n", str, (float)time / tps);
}