#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#ifdef STRESS
#include "stress_test.h"
#endif

// 声明时间打印输出的函数
void time_print(char *, clock_t);

int main(int argc, char *argv[]) {
    // clock_t 表示进程所用 CPU
    // 时间，单位是时钟周期，这里声明开始和结束时使用的时钟周期，用于表示进程使用的真实时间
    clock_t start, end;
    // struct tms 也用于衡量 CPU
    // 时间，但是一个结构体类型，用于表示程序在用户、系统模式下分别使用的 CPU
    // 时间，以及程序在开始运行后经历的时钟数，这里声明程序计算开始和结束使用的
    // CPU 时间的变量 该结构体有 4 个成员变量：
    // 1. tms_utime：表示程序在用户模式下所使用的CPU时间。
    // 2. tms_stime：表示程序在系统模式下所使用的CPU时间。
    // 3.
    // tms_cutime：表示由当前进程和它的子进程在用户模式下所使用的CPU时间之和。
    // 4.
    // tms_cstime：表示由当前进程和它的子进程在系统模式下所使用的CPU时间之和。
    struct tms t_start, t_end;
    // 标准 C 库中，times 函数用于获取一个 struct tms
    // 类型的值（参数中传指针填充），填充不同模式下使用的 CPU
    // 时间，并返回进程启动后经历的时钟数（真实时间）。
    start = times(&t_start);
#ifdef STRESS
    // fork 出子进程，也进行密集型计算
    pid_t pid = fork();
    if (pid == -1) {
        // fork 子进程失败
        perror("fork failed!\n");
        return -1;
    } else if (pid == 0) {
        // 子进程
        pid_t me = getpid();
        int m = 1000, n = 2000, k = 4000;
        printf("I am child %d, use m = %d, n = %d, k = %d\n", me, m, n, k);
        matrix_cal(m, n, k);
        return 0;
    } else {
        // 父进程
        pid_t me = getpid();
        int m = 2000, n = 4000, k = 8000;
        printf("I am parent %d, child %d, use m = %d, n = %d, k = %d\n", me,
               pid, m, n, k);
        matrix_cal(m, n, k);
    }
#else
    system("grep the /usr/doc/*/* > /dev/null 2> /dev/null");
#endif
    // tms 结构体在子进程结束时才更新，因此在生成前进行等待子进程退出结束
    int status;
    waitpid(-1, &status, 0);
    // 获取计算结束时进程计算经历的时钟数（end），并填充不同模式下使用的 CPU
    // 时间（t_end）
    end = times(&t_end);

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

    exit(EXIT_SUCCESS);
}

void time_print(char *str, clock_t time) {
    // 获取系统每秒所包含的 CPU
    // 时钟数（时钟滴答数，即一秒内有多少个时钟周期），用 sysconf
    // 函数获取符合实际。
    long tps = sysconf(_SC_CLK_TCK);
    // 根据每秒时钟数和运行时钟数算出精确经历的秒数
    printf("%s: %6.2f secs\n", str, (float)time / tps);
}