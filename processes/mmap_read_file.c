#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
int main(int argc, char* argv[]) {
    int fd;
    void *start;
    struct stat sb;
    /* 打开/etc/passwd */
    // fd = open("/etc/passwd", O_RDONLY); 
    /* 处于安全考虑，打开 /etc/resolv.conf */
    fd = open("/etc/resolv.conf", O_RDONLY);
    fstat(fd, &sb);                     /*取得文件大小*/
    /* 利用man fstat 可以看到struct stat 的定义*/
    start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (start == MAP_FAILED) /*判断是否映射成功*/
        return EXIT_FAILURE;
    printf("%s", start);
    munmap(start, sb.st_size); /*解除映射*/
    close(fd);
}