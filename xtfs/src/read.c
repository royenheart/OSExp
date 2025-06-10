/**
 * @file read.c
 * @author RoyenHeart
 * @brief 基础 XTFS 文件系统分区文件内容读取
 * @version 1.0.0
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "lex/folder_lex.h"
#include "xtfs_check.h"
#include "xtfs_limits.h"
#include "xtfs_manage.h"
#include "xtfs_struct.h"

// inode表
struct inode inode_table[NR_INODE];
// 文件系统分区文件名
char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
// 文件系统分区文件索引
FILE *fp_xtfs = NULL;

int main(int argc, char *argv[]) {
    char **dirnames = NULL;
    int dir_num;
    int i;
    // 目录所需数据
    CATALOG dir_index_table[CATALOG_TABLE_SIZE];
    // 文件所需数据
    int inode_blocknr;
    size_t filesize;
    int type;
    INDEX_TABLE_STRUC index_table_blocknr;
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    INDEX_TABLE_STRUC exist;

    dir_num = get_folders(argv[1], &dirnames);

    if (dir_num == ERROR_PARSE) {
        printf("read failed, check the input file format!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    type = get_file_type(atoi(argv[2]));
    if (is_same_type_class(type, DIR_FILE) == 1) {
        printf("Can't read a dir!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    check_fs_name(argv[3]);

    strncpy(fs_name, argv[3], MAX_FS_NAME_LENGTH);

    fp_xtfs = fopen(fs_name, "r");

    read_file(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);

    // 查找文件
    inode_blocknr = get_root_inode(inode_table);
    if (inode_blocknr == NOT_FOUND) {
        printf(
            "Root has been destroyed! This file system may not be in secure "
            "state!\n");
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }
    index_table_blocknr = inode_table[inode_blocknr].index_table_blocknr;
    read_dir_index_table(fp_xtfs, dir_index_table,
                         index_table_blocknr * BLOCK_SIZE);

    for (i = 0; i <= dir_num; i++) {
        int child_in_father_index;
        child_in_father_index = find_dir_index_table(
            dirnames[i], dir_index_table, (i < dir_num) ? DIR_FILE : type);
        if (child_in_father_index == NOT_FOUND) {
            printf("No such file %s with type %d!\n", argv[1], type);
            fclose(fp_xtfs);
            xtfs_exit(EXIT_FAILURE);
        } else {
            int curr_inode = dir_index_table[child_in_father_index].pos;
            index_table_blocknr = inode_table[curr_inode].index_table_blocknr;
            if (i < dir_num) {
                read_dir_index_table(fp_xtfs, dir_index_table,
                                     index_table_blocknr * BLOCK_SIZE);
            } else {
                read_index_table(fp_xtfs, index_table,
                                 index_table_blocknr * BLOCK_SIZE);
            }
            inode_blocknr = curr_inode;
        }
    }

    // 读取文件大小
    filesize = inode_table[inode_blocknr].size;
    // 数据块内容已经存储的数据块
    exist = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // 根据数据块索引表读入文件数据
    for (i = 0; i < exist; i++) {
        // 防止缓存区溢出
        char data[BLOCK_SIZE + 1] = {0};
        // 检查 index_table ，得到应该读取的正确位置，否则转换到下一个
        INDEX_TABLE_STRUC index = i;
        if (index && index % INDEX_TABLE_DATA_SIZE == 0) {
            index_table_blocknr = index_table[INDEX_TABLE_DATA_SIZE];
            read_index_table(fp_xtfs, index_table,
                             index_table_blocknr * BLOCK_SIZE);
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
