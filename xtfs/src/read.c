/**
 * @file read.c
 * @author RoyenHeart
 * @brief 基础文件系统数据读入
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
#include "xtfs_struct.h"
#include "xtfs_manage.h"
#include "xtfs_check.h"
#include "io.h"

#ifdef DEBUG
#include <assert.h>
#endif

// inode表
struct inode inode_table[NR_INODE];
// 文件系统文件名
char* fs_name = NULL;
// 文件系统文件索引
FILE* fp_xtfs = NULL;

int main(int argc, char* argv[]) {
    char filename[MAX_FILE_NAME_LENGTH] = {0};
    // 数据块索引表
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    int i;

    check_file_name(argv[1]);
    check_fs_name(argv[2]);

    strcpy(filename, argv[1]);
    fs_name = argv[2];
    fp_xtfs = fopen(fs_name, "r");
#ifdef DEBUG
    assert(fp_xtfs != NULL);
#endif
    read_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);
    for (i = 0; i < NR_INODE; i++) {
        // 未加入目录，目前只以文件名判断且不是空文件（文件类型）
        if (strcmp(inode_table[i].filename, filename) == 0 && inode_table[i].type != NO_FILE) {
            long int blocknr = inode_table[i].index_table_blocknr;
            read_file(fp_xtfs, blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            break;
        }
    }

    if (i == NR_INODE) {
        printf("No such file: %s\n", filename);
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }

    // 根据数据块索引表读入文件数据
    for (i = 0; i < BLOCK_SIZE / 2; i++) {
        // 防止缓存区溢出
        char data[BLOCK_SIZE + 1] = {0};
        int block_pos = index_table[i];
        if (block_pos == 0) {
            break;
        }
        read_file(fp_xtfs, block_pos * BLOCK_SIZE, data, BLOCK_SIZE);
        printf("%s", data);
    }

    fclose(fp_xtfs);
    return (EXIT_SUCCESS);
}