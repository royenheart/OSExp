/**
 * @file format.c
 * @author RoyenHeart, Ruoxuan Wang, MDND, bow
 * @brief 格式化镜像、分区文件系统
 * @version 0.1
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xtfs_limits.h"
#include "xtfs_check.h"
#include "xtfs_struct.h"

/**
 * @brief 格式化
 *
 * @param argc 参数个数
 * @param argv 参数内容
 * @return int 0 成功格式化
 */
int main(int argc, char* argv[]) {
    FILE* fp = NULL;
    char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
    char blank[BLOCK_SIZE] = {0};

    check_fs_name(argv[1]);

    strncpy(fs_name, argv[1], MAX_FS_NAME_LENGTH);

    // 以可读可写模式打开xtfs.img文件
    fp = fopen(fs_name, "r+");
    // 覆盖之前的 inode 表和数据块位图
    fwrite(blank, 1, BLOCK_SIZE, fp);
    fwrite(blank, 1, BLOCK_SIZE, fp);
    // 将根目录和其占用的数据块索引表写入
    struct inode root;
    memset(root.filename, 0, MAX_FILE_NAME_LENGTH);
    strncpy(root.filename, "/", MAX_FILE_NAME_LENGTH);
    root.index_table_blocknr = 2;
    // 目录的 size 表示占用了多少块数据块索引表
    root.size = 1;
    root.type = DIR_FILE;
    fseek(fp, 0, SEEK_SET);
    fwrite((char*)&root, 1, sizeof(root), fp);
    fseek(fp, 2 * BLOCK_SIZE, SEEK_SET);
    fwrite(blank, 1, BLOCK_SIZE, fp);
    // 设置初始数据块位图，0，1，2三个数据块都被占用（2被根目录占用）
    fseek(fp, BLOCK_SIZE, SEEK_SET);
    fputc(7, fp);
    fclose(fp);

    return (EXIT_SUCCESS);
}

