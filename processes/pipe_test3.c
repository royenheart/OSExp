#include <stdio.h>

int main(int argc, char* argv[]) {
    FILE *fp;
    char buffer[80];
    // 创建子进程执行 cat /etc/passwd 文件，并获取子进程的标准输出文件指针（获取管道IO）
    fp = popen("cat /etc/passwd", "r");
    // 从子进程的标准输出文件指针中读取输出
    fgets(buffer, sizeof(buffer), fp);
    // 获取子进程执行的输出
    printf("%s", buffer);
    // 关闭子进程标准输出文件的文件指针
    pclose(fp);
}