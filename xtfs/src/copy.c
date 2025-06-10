/**
 * @file copy.c
 * @author RoyenHeart, ovovcast, MDND, bow
 * @brief 基础 XTFS 文件系统分区文件拷贝
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

// 读取的inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
// 数据块位图lowbit表
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
// 文件系统分区文件名
char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
// 文件系统分区文件索引
FILE *fp_xtfs;

int main(int argc, char *argv[]) {
    char **dirnames = NULL;
    int dir_num;
    int i;
    // 目录信息
    CATALOG dir_index_table[CATALOG_TABLE_SIZE];
    // 文件信息
    int inode_blocknr;
    size_t filesize;
    int type;
    INDEX_TABLE_STRUC index_table_blocknr;

    // 初始化lowbit表
    for (i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    dir_num = get_folders(argv[1], &dirnames);

    if (dir_num == ERROR_PARSE) {
        printf("copy failed, check the input file format!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    check_fs_name(argv[3]);
    type = get_file_type(atoi(argv[2]) & ~SPEC_TYPES);
    if (is_same_type_class(type, DIR_FILE) == 1) {
        printf("Can't copy all folder right now!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    strncpy(fs_name, argv[3], MAX_FS_NAME_LENGTH);

    fp_xtfs = fopen(fs_name, "r+");

    // 读取0号和1号inode表和数据块位图数据到进程管理的内存（数组），便于修改和之后的写入
    read_first_two_blocks(fp_xtfs, inode_table, block_map);

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

    for (i = 0; i < dir_num; i++) {
        int child_in_father_index;
        child_in_father_index =
            find_dir_index_table(dirnames[i], dir_index_table, DIR_FILE);
        if (child_in_father_index == NOT_FOUND) {
            printf("No such dir to copy!\n");
            fclose(fp_xtfs);
            xtfs_exit(EXIT_FAILURE);
        } else {
            int curr_inode = dir_index_table[child_in_father_index].pos;
            index_table_blocknr = inode_table[curr_inode].index_table_blocknr;
            read_dir_index_table(fp_xtfs, dir_index_table,
                                 index_table_blocknr * BLOCK_SIZE);
            inode_blocknr = curr_inode;
        }
    }

    // 在inode表中申请一个空闲inode，存放文件的inode信息
    inode_blocknr = get_empty_inode(inode_table, dirnames[dir_num], type);
    // 在目录中添加对应的表项，并重新写回文件系统分区
    get_empty_dir_index(dir_index_table, dirnames[dir_num], type,
                        inode_blocknr);
    write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE,
               (char *)dir_index_table, CATALOG_TABLE_SIZE * sizeof(CATALOG));

    // 将文件中内容拷贝到xtfs文件系统分区中
    FILE *fp = fopen(argv[1], "r");
    filesize = read_file_size(fp);
    normal_params_load(fp);
    index_table_blocknr =
        copy_blocks(filesize, type, block_map, lowbit, fp_xtfs);
    fclose(fp);
    // 文件复制准备完全后将数据同步至inode表
    inode_table[inode_blocknr].size = filesize;
    inode_table[inode_blocknr].index_table_blocknr = index_table_blocknr;
    // 将0号和1号数据块内容写回并关闭文件系统分区
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}