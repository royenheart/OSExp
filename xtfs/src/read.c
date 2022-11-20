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
char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
// 文件系统文件索引
FILE* fp_xtfs = NULL;

int main(int argc, char* argv[]) {
    char filename[MAX_FILE_NAME_LENGTH + 1] = {0};
    INDEX_TABLE_STRUC index_table_blocknr;
    // 数据块索引表
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    INDEX_TABLE_STRUC exist;
    int i;
    int filesize;

    check_file_name(argv[1]);
    check_fs_name(argv[2]);

    strncpy(filename, argv[1], MAX_FILE_NAME_LENGTH);
    strncpy(fs_name, argv[2], MAX_FS_NAME_LENGTH);
    
    fp_xtfs = fopen(fs_name, "r");

    #ifdef DEBUG
    assert(fp_xtfs != NULL);
    #endif

    read_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);

    int inode_index_table = find_inode_table(filename, inode_table);
    if (inode_index_table == NOT_FOUND) {
        // 返回-1表示文件不存在
        printf("The file %s does not exist!\n", filename); 
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }
    // 得到第一个数据块索引表
    index_table_blocknr = inode_table[inode_index_table].index_table_blocknr;
    // read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
    read_index_table(fp_xtfs, index_table, index_table_blocknr * BLOCK_SIZE);
    // 读取文件大小
    filesize = inode_table[inode_index_table].size;
    // 数据块内容已经存储的数据块
    exist = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // for (i = 0; i < NR_INODE; i++) {
    //     // 未加入目录，目前只以文件名判断且不是空文件（文件类型）
    //     if (strcmp(inode_table[i].filename, filename) == 0 && inode_table[i].type != NO_FILE) {
    //         index_table_blocknr = inode_table[i].index_table_blocknr;
    //         read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
    //         break;
    //     }
    // }

    // if (i == NR_INODE) {
    //     printf("No such file: %s\n", filename);
    //     fclose(fp_xtfs);
    //     xtfs_exit(EXIT_FAILURE);
    // }

    // 根据数据块索引表读入文件数据
    for (i = 0; i < exist; i++) {
        // 防止缓存区溢出
        char data[BLOCK_SIZE + 1] = {0};
        // 检查 index_table ，得到应该读取的正确位置，否则转换到下一个
        INDEX_TABLE_STRUC index = i;
        if (index && index % INDEX_TABLE_DATA_SIZE == 0) {
            index_table_blocknr = index_table[INDEX_TABLE_DATA_SIZE];
            // memset(index_table, 0, INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
            // read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            read_index_table(fp_xtfs, index_table, index_table_blocknr * BLOCK_SIZE);
            index = 0;
        } else {
            index = index % INDEX_TABLE_DATA_SIZE;
        }
        INDEX_TABLE_STRUC block_pos = index_table[index];
        if (block_pos == 0) {
            break;
        }
        read_file(fp_xtfs, block_pos * BLOCK_SIZE, data, BLOCK_SIZE);
        printf("%s", data);
    }

    fclose(fp_xtfs);
    return (EXIT_SUCCESS);
}