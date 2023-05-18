#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    // 读取具名管道文件名称
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char* FIFO = argv[1];

    // 读入缓冲区
    char buffer[80] = {0};
    int fd;

    // 删除已有具名管道文件
    unlink(FIFO);
    // 创建具名管道文件，并设置权限666（可读写）
    mkfifo(FIFO, 0666);
    int pid = fork();
    if (pid > 0) { // 父进程
        char s[] = "hello!\n";
        // 父进程打开管道文件
        fd = open(FIFO, O_WRONLY);
        // 父进程向管道文件写数据
        write(fd, s, sizeof(s));
        // 父进程写完毕，关闭管道文件
        close(fd);
    } else { // 子进程
        fd = open(FIFO, O_RDONLY);
        // 子进程从管道文件中读出父进程写入的数据，写入缓冲区
        read(fd, buffer, 80);
        // 打印得到的数据
        printf("%s", buffer);
        // 关闭管道文件
        close(fd);
    }
}