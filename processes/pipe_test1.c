#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int filedes[2];
    char buffer[80];
    // 创建管道
    if (pipe(filedes) < 0)
        printf("pipe error");
    // fork 父子进程
    if (fork() > 0) { // 父进程
        // 设置信息
        char s[] = "hello!\n";
        // 关闭读入端
        close(filedes[0]);
        // 向写入端传递信息
        write(filedes[1], s, sizeof(s));
        // 关闭写入端
        close(filedes[1]);
    } else {
        // 关闭写入端
        close(filedes[1]);
        // 从读入端接收信息
        read(filedes[0], buffer, 80);
        // 打印信息
        printf("%s", buffer);
        // 关闭读入端
        close(filedes[0]);
    }
}