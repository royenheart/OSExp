#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int fd1[2], fd2[2];
    pid_t pid;

    // 读取文件名称
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    // 创建管道
    if (pipe(fd1) == -1) {
        perror("pipe1 open failed");
        exit(EXIT_FAILURE);
    }

    if (pipe(fd2) == -1) {
        perror("pipe2 open failed");
        exit(EXIT_FAILURE);
    }

    // 创建子进程
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {  // 父进程
        // 建立双向数据传输管道
        close(fd1[0]);
        close(fd2[1]);

        // 读取数据文件
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            perror("file open error");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];
        while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
            int data = atoi(buffer);  // 将读取的数据转换为整数
            printf("Father read number %d from file\n", data);

            // 将数据发送到子进程进行计算
            printf("Father send number %d to child\n", data);
            write(fd1[1], &data, sizeof(int));
        }

        // 关闭向子进程传递数据的写入端
        close(fd1[1]);

        int r;
        while (read(fd2[0], &r, sizeof(int)) > 0) {
            // 将计算结果发送到父进程显示
            printf("Father get Result: %d\n", r);
        }

        // 关闭从子进程读入数据的读入端
        close(fd2[0]);

        // 等待子进程结束
        wait(NULL);

        // 关闭文件
        fclose(file);
    } else {  // 子进程
        // 建立双向数据传输管道
        close(fd1[1]);
        close(fd2[0]);

        int data;
        while (read(fd1[0], &data, sizeof(int)) > 0) {
            printf("Child get data: %d\n", data);
            int result = data * 4;

            // 将计算结果发送到父进程显示
            printf("Child send result: %d\n", result);
            write(fd2[1], &result, sizeof(int));
        }

        close(fd1[0]);
        close(fd2[1]);
    }

    return 0;
}
