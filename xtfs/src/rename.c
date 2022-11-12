/**
 * @file rename.c
 * @author Ruoxuan Wang
 * @brief 基础文件系统重命名
 * @version 0.1
 * @date 2022-11-8
 *
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xtfs_limits.h"
#include "xtfs_struct.h"
#include "xtfs_manage.h"
#include "io.h"

// inode表
struct inode inode_table[NR_INODE];
// 文件系统文件名
char* fs_name = NULL;
// 文件系统文件索引
FILE* fp_xtfs = NULL;

int main(int argc, char* argv[]) {
    char filename[MAX_FILE_NAME_LENGTH] = {0};
    char fileRename[MAX_FILE_NAME_LENGTH] = {0};
    int i;

    if (strlen(argv[1]) > MAX_FILE_NAME_LENGTH) {
        printf("File name too long!: %s\n", argv[1]);
        xtfs_exit(EXIT_FAILURE);
    }
    if (strlen(argv[2]) > MAX_FILE_NAME_LENGTH) {
        printf("New file name too long!: %s\n", argv[2]);
        xtfs_exit(EXIT_FAILURE);
    }
    if (strlen(argv[3]) > MAX_FS_NAME_LENGTH) {
        printf("XTFS name too long!: %s\n", argv[3]);
        xtfs_exit(EXIT_FAILURE);
    }

    strcpy(filename, argv[1]);
    strcpy(fileRename, argv[2]);
    fs_name = argv[3];
    fp_xtfs = fopen(fs_name, "r+");

    // 读取inode_table
    read_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);

    // 以文件名判断是否存在inode_table中
    for (i = 0; i < NR_INODE; i++) {
        if (strcmp(inode_table[i].filename, filename) == 0) {
            strcpy(inode_table[i].filename, fileRename);
            break;
        }
    }

    // 未找到该文件
    if (i == NR_INODE) {
        printf("No such file: %s\n", filename);
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }

    // 将inode表写回0号数据块
    write_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);
    // 关闭文件系统
    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}