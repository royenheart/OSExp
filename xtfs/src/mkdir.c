/**
 * @file mkdir.c
 * @author RoyenHeart
 * @brief 创建目录
 * @version 0.1
 * @date 2022-11-19
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

// inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
// 数据块位图lowbit表
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
// 文件系统文件名
char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
// 文件系统文件索引
FILE* fp_xtfs = NULL;

int main(int argc, char* argv[]) {
    char **dirnames = NULL;
    char blank[BLOCK_SIZE] = {0};
    int dir_num;
    int i;
    int father_inode;
    INDEX_TABLE_STRUC father_index_table_blocknr;
    // 当前需要查找的数据块索引表（按照目录的结构）
    CATALOG index_table[CATALOG_TABLE_SIZE];

    check_fs_name(argv[2]);

    dir_num = get_folders(argv[1], &dirnames);
    
    if (dir_num == ERROR_PARSE) {
        printf("mkdir failed, check the input file format!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    // 初始化lowbit表
    for (i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    strncpy(fs_name, argv[2], MAX_FS_NAME_LENGTH);
    
    fp_xtfs = fopen(fs_name, "r+");

    read_first_two_blocks(fp_xtfs, inode_table, block_map);

    father_inode = get_root_inode(inode_table);
    if (father_inode == NOT_FOUND) {
        printf("Root has been destroyed! This file system may not be in secure state!\n"); 
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }
    // 得到根目录的第一个数据块索引表
    father_index_table_blocknr = inode_table[father_inode].index_table_blocknr;
    read_dir_index_table(fp_xtfs, index_table, father_index_table_blocknr * BLOCK_SIZE);
    // 读取文件大小（后面目录支持二级索引，需要进行占用数据块索引表大小的变更）
    // filesize = inode_table[inode_index_table].size;

    for (i = 0; i <= dir_num; i++) {
        int child_in_father_index;
        child_in_father_index = find_dir_index_table(dirnames[i], index_table, DIR_FILE);
        // 找到和未找到的处理
        if (child_in_father_index == NOT_FOUND) {
            // 创建目录，申请 index_table 和 inode 表空间并填充相应数据
            short *new_index = get_all_block(1, block_map, lowbit);
            int new_inode = get_empty_inode(inode_table, dirnames[i], DIR_FILE);
            child_in_father_index = get_empty_dir_index(index_table, dirnames[i], DIR_FILE, new_inode);
            // 由于目录的 index table 块处于和普通文件共享的数据区，且目录需要从此块读取数字，因此每次申请到新的 index table 空间，都需要将其覆盖，防止之前的数据的影响
            write_file(fp_xtfs, new_index[0] * BLOCK_SIZE, blank, BLOCK_SIZE);
            inode_table[new_inode].index_table_blocknr = new_index[0];
            inode_table[new_inode].size = 1;
            // 申请和数据填充完成，将父目录的 index_table 写入文件系统
            // 这里改成全部空间全部申请完成后再写入会好点，但是与现有实现功能不是很匹配，只能先这样写，后面若有更好的解决方案进行修正
            write_file_with_blank(fp_xtfs, father_index_table_blocknr * BLOCK_SIZE, (char*)index_table, CATALOG_TABLE_SIZE * sizeof(CATALOG));
            // 进行迭代
            father_inode = new_inode;
            father_index_table_blocknr = new_index[0];
            read_dir_index_table(fp_xtfs, index_table, father_index_table_blocknr * BLOCK_SIZE);
        } else {
            // 获取 inode 表项
            int curr_inode = index_table[child_in_father_index].pos;
            father_index_table_blocknr = inode_table[curr_inode].index_table_blocknr;
            // 进行迭代
            read_dir_index_table(fp_xtfs, index_table, father_index_table_blocknr * BLOCK_SIZE);
            father_inode = curr_inode;
        }
    }

    // 全部生成完毕，将 inode 表和 block map 写入文件系统
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}