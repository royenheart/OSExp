#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

/*
编写一个程序，实现将一个目录的所有内容复制到另一个目录的功能。要求：
原文件（目录）和目标文件（目录）的属主、权限等信息保持一致；
每复制一个文件（目录），在屏幕提示相应信息；
当遇到符号链接文件时，显示该文件为链接文件，不复制。
*/

/// @brief 拷贝目录
/// @param src_dir 原目录
/// @param dest_dir 目标目录
void copy_directory(const char *src_dir, const char *dest_dir);

/// @brief 拷贝文件
/// @param src_file 原文件  
/// @param dest_file 目标文件
void copy_file(const char *src_file, const char *dest_file);

int main(int argc, char* argv[]) {
    // 读取目录名称
    if (argc < 2) {
        printf("Usage: %s <src_dir> <dest_dir>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *src_dir = argv[1];
    const char *dest_dir = argv[2];

    copy_directory(src_dir, dest_dir);

    return EXIT_SUCCESS;
}

void copy_directory(const char *src_dir, const char *dest_dir) {
    DIR *dir = opendir(src_dir);

    // 打开原目录
    if (dir == NULL) {
        perror("无法打开原目录");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    // 不断获取原目录各个入口点（文件、目录，进行判断）
    while ((entry = readdir(dir)) != NULL) {
        // 跳过当前目录和上级目录（. 和 ..）
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 设置原文件路径
        char src_path[1024];
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);

        // 设置目标文件路径
        char dest_path[1024];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, entry->d_name);

        struct stat st;
        // 获取文件状态
        if (lstat(src_path, &st) == -1) {
            perror("无法获取文件状态");
            exit(EXIT_FAILURE);
        }

        // 判断是否为符号链接
        if (S_ISLNK(st.st_mode)) {
            printf("符号链接，跳过: %s\n", src_path);
            continue;
        }

        // 判断是否为目录
        if (S_ISDIR(st.st_mode)) {
            if (mkdir(dest_path, st.st_mode) == -1) {
                perror("无法创建目标目录，无法继续进行拷贝");
                exit(EXIT_FAILURE);
            }
            // 当为目录，且目标对应目录创建完毕，进行拷贝目录操作（递归调用）
            copy_directory(src_path, dest_path);
        } else {
            // 当为文件时，进行拷贝
            copy_file(src_path, dest_path);
        }
    }

    // 全部操作完毕，关闭目录
    if (closedir(dir) != 0) {
        printf("无法正确关闭目录: %s", dir);
        exit(EXIT_FAILURE);
    }
}

void copy_file(const char *src_file, const char *dest_file) {
    // 以只读模式打开
    int src_fd = open(src_file, O_RDONLY);
    if (src_fd == -1) {
        perror("无法打开原文件");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    // 获取文件状态
    if (fstat(src_fd, &st) == -1) {
        perror("无法获取文件状态");
        exit(EXIT_FAILURE);
    }

    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);
    if (dest_fd == -1) {
        perror("无法创建目标文件");
        exit(EXIT_FAILURE);
    }

    // 按字节不断读取原文件内容，根据文件描述符，写入目标文件
    char buffer[1024];
    ssize_t bytes_read, bytes_written;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written == -1) {
            perror("无法写入目标文件");
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read == -1) {
        perror("无法读取原文件");
        exit(EXIT_FAILURE);
    }

    if (close(src_fd) == -1) {
        perror("无法关闭原文件");
        exit(EXIT_FAILURE);
    }

    if (close(dest_fd) == -1) {
        perror("无法关闭目标文件");
        exit(EXIT_FAILURE);
    }

    printf("拷贝文件: %s\n", src_file);
}
