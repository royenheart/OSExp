#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

long PAGE_SIZE = 4096;

/*
读取文件内容
*/
void display_page(char *addr, off_t offset, size_t size) {
    for (size_t i = 0; i < size; i++) {
        putchar(addr[offset + i]);
    }
}

int main(int argc, char *argv[]) {
    // 读取文件名称
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    PAGE_SIZE = sysconf(_SC_PAGE_SIZE);

    // 打开文件
    char *filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Cannot open file");
        return 1;
    }

    // 读取文件状态信息
    struct stat file_stat;
    // 通过 fstat 通过 fs 文件描述词（打开标识符）获取文件状态
    if (fstat(fd, &file_stat) == -1) {
        perror("Unable to get file size");
        close(fd);
        return 1;
    }

    // 获取文件大小
    off_t file_size = file_stat.st_size;
    // 对文件建立内存映射
    char *addr = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        printf("Unable to map file into memory\n");
        switch (errno) {
            case EBADE:
                perror("fd: not a valid file descriptor");
                break;
            case EACCES:
                perror(
                    "Incorrect access permissions. If it is MAP_PRIVATE, the "
                    "file "
                    "must be readable, and if MAP_SHARED is used, PROT_WRITE "
                    "is "
                    "required and the file must be able to be written.");
                break;
            case EINVAL:
                perror("Not valid: start or length or offset");
                break;
            case EAGAIN:
                perror("File locked, or memlock too much");
                break;
            case ENOMEM:
                perror("No mem left");
                break;
            default:
                break;
        }
        close(fd);
        return 1;
    }

    // 当前读取的偏移值
    off_t offset = 0;
    // 还未读取的文件大小
    size_t remaining_bytes = file_size;
    // 持续读取文件直至读取完毕
    while (remaining_bytes > 0) {
        // 每次读取页面大小或剩下的页面
        size_t page_bytes =
            remaining_bytes < PAGE_SIZE ? remaining_bytes : PAGE_SIZE;
        // 打印输出
        display_page(addr, offset, page_bytes);
        // 偏移量加上这一次读取的大小
        offset += page_bytes;
        // 减少剩下该读取的文件大小
        remaining_bytes -= page_bytes;

        // 终端输入 q ，中断读取
        int input = getchar();
        if (input == 'q') {
            break;
        }
    }

    // 接触内存映射
    munmap(addr, file_size);
    // 通过文件标识符关闭文件
    close(fd);

    return 0;
}
