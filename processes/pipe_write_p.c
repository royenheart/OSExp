#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER 80
#define ASCII_RANDOM (rand() % 95 + 32)
#define TIME_RANDOM (rand() % 5 + 1)

int main(int argc, char* argv[]) {
    // 读取具名管道文件名称
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    srand((unsigned)time(NULL));
    char* FIFO = argv[1];
    char buffer[BUFFER] = {0};
    const int BUFFER_SIZE = BUFFER * sizeof(char);
    int myid = getpid();

    int fd = open(FIFO, O_WRONLY);
    for(int i = 0; i < 10; i++) {
        char s[20] = {0};
        for (int j = 0; j < 20; j++) {
            s[j] = ASCII_RANDOM;
        }
        sprintf(buffer, "%d:::%s\n", myid, s);
        write(fd, buffer, BUFFER_SIZE);
        memset(buffer, 0, BUFFER_SIZE);
        sleep(TIME_RANDOM);
    }
    // 关闭管道文件
    close(fd);   
}