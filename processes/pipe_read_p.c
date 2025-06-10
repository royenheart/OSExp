#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 80

int main(int argc, char *argv[]) {
    // 读取具名管道文件名称
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *FIFO = argv[1];

    // 读入缓冲区
    char buffer[SIZE] = {0};
    const int BUFFER_SIZE = SIZE * sizeof(char);
    int fd;

    // 删除已有具名管道文件
    unlink(FIFO);
    // 创建具名管道文件，并设置权限666（可读写）
    mkfifo(FIFO, 0666);

    fd = open(FIFO, O_RDONLY);
    while (1) {
        // 子进程从管道文件中读出父进程写入的数据，写入缓冲区
        int g = read(fd, buffer, BUFFER_SIZE);
        // 打印得到的数据
        if (g != -1) {
            printf("%s", buffer);
            memset(buffer, 0, BUFFER_SIZE);
        }
    }
    // 关闭管道文件
    close(fd);
}