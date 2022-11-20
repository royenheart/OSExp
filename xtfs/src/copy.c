/**
 * @file copy.c
 * @author RoyenHeart, Ruoxuan Wang, MDND, bow
 * @brief 基础文件系统文件拷贝
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

// 读取的inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
// 数据块位图lowbit表
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
// 文件系统文件名
char* fs_name = NULL;
// 文件系统文件索引
FILE fp_xtfs[MAX_FS_NAME_LENGTH + 1] = {0};

int main(int argc, char* argv[]) {
    // INIT_XTFS_MANAGE
    size_t filesize;
    short index_table_blocknr;
    // 文件的数据块索引表，数据块索引表用于记录文件在此文件系统中占用了哪些数据块，存放该文件占用的所有数据块的块号
    char filename[MAX_FILE_NAME_LENGTH + 1] = {0};
    int type;
    int i;

    // 初始化lowbit表
    for (i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    check_file_name(argv[1]);
    check_fs_name(argv[3]);

    // 获取待拷贝文件名和文件类型
    strncpy(filename, argv[1], MAX_FILE_NAME_LENGTH);
    // 去除特定格式
    type = get_file_type(atoi(argv[2]) & ~SPEC_TYPES);
    strncpy(fs_name, argv[3], MAX_FS_NAME_LENGTH);

    fp_xtfs = fopen(fs_name, "r+");

    // 读取0号和1号inode表和数据块位图数据到进程管理的内存（数组），便于修改和之后的写入
    read_first_two_blocks(fp_xtfs, inode_table, block_map);
    // 在inode表中申请一个空闲inode，存放文件的inode信息，若存在相同文件，退出（之后版本可能添加调用覆写程序）
    i = get_empty_inode(inode_table, filename, type);
    // 将文件中内容拷贝到xtfs文件系统中
    FILE *fp = fopen(filename, "r");
    filesize = read_file_size(fp);
    normal_params_load(fp);
    index_table_blocknr = copy_blocks(filesize, type, block_map, lowbit, fp_xtfs);
    fclose(fp);
    // filesize = copy_blocks(filename, index_table);
    // 将数据块索引表拷贝到xtfs文件系统中
    // index_table_blocknr = write_index_table(fp_xtfs, block_map, lowbit, index_table);
    // 文件复制准备完全后将数据同步至inode表
    inode_table[i].size = filesize;
    inode_table[i].index_table_blocknr = index_table_blocknr;
    // 将0号和1号数据块内容写回并关闭文件系统文件
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}