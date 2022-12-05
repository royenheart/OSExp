/**
 * @file delete.c
 * @author bow
 * @brief XTFS 文件系统分区文件删除
 * @version 1.0.0
 * @date 2022-11-14
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
#include "lex/folder_lex.h"

// inode 表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
// 文件系统分区文件名
char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
// 文件系统分区文件索引
FILE* fp_xtfs = NULL;

int main(int argc, char* argv[]) {
    char** dirnames = NULL;
    int dir_num;
    int i;
    // 目录信息
    CATALOG dir_index_table[CATALOG_TABLE_SIZE];
    // 文件信息
    int inode_blocknr;
    size_t filesize;
    int type;
    INDEX_TABLE_STRUC index_table_blocknr;
    INDEX_TABLE_STRUC exist;
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};

    check_fs_name(argv[3]);

    // 提取文件路径
    dir_num = get_folders(argv[1], &dirnames);

    if (dir_num == ERROR_PARSE) {
        printf("delete failed, check the input file format!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    type = get_file_type(atoi(argv[2]));
    if (is_same_type_class(type, DIR_FILE) == 1) {
        printf("Can't delete a dir!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    strncpy(fs_name, argv[3], MAX_FS_NAME_LENGTH);

    fp_xtfs = fopen(fs_name, "r+");

    read_first_two_blocks(fp_xtfs, inode_table, block_map);

    // 获取根节点
    inode_blocknr = get_root_inode(inode_table);
    if (inode_blocknr == NOT_FOUND) {
        printf("Root has been destroyed! This file system may not be in secure state!\n");
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }
    // 得到根目录的第一个数据块索引表
    index_table_blocknr = inode_table[inode_blocknr].index_table_blocknr;

    // 读取目录类型文件的数据块索引表(manage)，存到dir_index_table中
    read_dir_index_table(fp_xtfs, dir_index_table, index_table_blocknr * BLOCK_SIZE);

    for (i = 0; i <= dir_num; i++) {
        int child_in_father_index = find_dir_index_table(dirnames[i], dir_index_table, (i < dir_num) ? DIR_FILE : type);
        if (child_in_father_index == NOT_FOUND) {
            printf("The file or path %s with type %d does not exist!\n", argv[1], type);
            fclose(fp_xtfs);
            xtfs_exit(EXIT_FAILURE);
        } else {
            INDEX_TABLE_STRUC temp = index_table_blocknr;
            inode_blocknr = dir_index_table[child_in_father_index].pos;
            index_table_blocknr = inode_table[inode_blocknr].index_table_blocknr;
            if (i < dir_num) {
                read_dir_index_table(fp_xtfs, dir_index_table, index_table_blocknr * BLOCK_SIZE);
            } else {
                // 去除对应目录中的文件（类型改为NO_FILE即可）
                dir_index_table[child_in_father_index].type = NO_FILE;
                write_file(fp_xtfs, temp * BLOCK_SIZE, (char*)dir_index_table, CATALOG_TABLE_SIZE * sizeof(CATALOG));
                read_index_table(fp_xtfs, index_table, index_table_blocknr * BLOCK_SIZE);
            }
        }
    }

    // 读取文件大小
    filesize = inode_table[inode_blocknr].size;
    // 数据块内容已经存储的数据块
    exist = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // 将第一个数据块索引表在数据块位图中置0
    set_block_map(0, index_table_blocknr, block_map);

    for (i = 0; i < exist; i++) {
        // 检查 index_table ，得到应该读取的正确位置，若当前 index_table 已经读满，则进入下一个。
        INDEX_TABLE_STRUC index = i;
        if (index && index % INDEX_TABLE_DATA_SIZE == 0) {
            index_table_blocknr = index_table[INDEX_TABLE_DATA_SIZE];
            read_index_table(fp_xtfs, index_table, index_table_blocknr * BLOCK_SIZE);
            set_block_map(0, index_table_blocknr, block_map);
            index = 0;
        } else {
            index = index % INDEX_TABLE_DATA_SIZE;
        }
        // 将所有占用的数据块在block_map中置0
        set_block_map(0, index_table[index], block_map);
    }

    // 将对应 inode 表类型置0，即表示为空文件
    inode_table[inode_blocknr].type = NO_FILE;

    // 删除完毕，将 inode 表和 block map 写入文件系统分区
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}