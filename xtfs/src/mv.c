/**
 * @file mv.c
 * @author ovovcast
 * @brief 基础文件系统剪切
 * @details 
 * 提供文件、目录剪切：(目标参数必须为目录)
 * 1. 源文件类型为目录，将源目录剪切至目标目录下，保留文件名
 * 2. 源文件类型为文件，将源文件剪切至目标目录下，保留文件名
 * @version 0.1
 * @date 2022-11-22
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

// 数据块位图lowbit表
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
// inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
// 文件系统文件名
char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
// 文件系统文件索引
FILE* fp_xtfs = NULL;

int main(int argc, char* argv[]) {
    char **dir_names = NULL, **dir_renames = NULL;
    int dir_num, dir_renum;
    int i;
    // 源文件信息
    int type;
    int file_father_inode;
    CATALOG dir_index_table[CATALOG_TABLE_SIZE];
    INDEX_TABLE_STRUC father_index_table_blocknr;
    // 目标文件信息
    int refile_father_inode;
    CATALOG redir_index_table[CATALOG_TABLE_SIZE];
    INDEX_TABLE_STRUC refather_index_table_blocknr;

    // 初始化lowbit表
    for (i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    check_fs_name(argv[4]);

    // 获取目录、文件结构
    dir_num = get_folders(argv[1], &dir_names);
    dir_renum = get_folders(argv[3], &dir_renames);
    
    // 路径错误
    if (dir_num == ERROR_PARSE || dir_renum == ERROR_PARSE) {
        printf("delete failed, check the input file format!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    type = get_file_type(atoi(argv[2]));
    
    strncpy(fs_name, argv[4], MAX_FS_NAME_LENGTH);
    fp_xtfs = fopen(fs_name, "r+");

    // 读取0号和1号inode表和数据块位图数据到进程管理的内存（数组），便于修改和之后的写入
    read_first_two_blocks(fp_xtfs, inode_table, block_map);

    // father_inode 为 root 根目录在 inode 表中的位置
    file_father_inode = get_root_inode(inode_table);
    if (file_father_inode == NOT_FOUND) {
        printf("Root has been destroyed! This file system may not be in secure state!\n"); 
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }

    // 得到根目录的数据块索引表
    father_index_table_blocknr = inode_table[file_father_inode].index_table_blocknr;
    read_dir_index_table(fp_xtfs, dir_index_table, father_index_table_blocknr * BLOCK_SIZE);

    // 查找源文件是否存在
    for (i = 0; i < dir_num; i++) {
        // 根据文件名，目录名和目录的 index table 找到对应的表项
        int child_in_father_index;
        child_in_father_index = find_dir_index_table(dir_names[i], dir_index_table, DIR_FILE);
        if (child_in_father_index == NOT_FOUND) {   
            printf("No such source dir %s!\n", argv[1]);
            fclose(fp_xtfs);
            xtfs_exit(EXIT_FAILURE);
        } else {
            // 获取 inode 表项
            file_father_inode = dir_index_table[child_in_father_index].pos;
            father_index_table_blocknr = inode_table[file_father_inode].index_table_blocknr;
            read_dir_index_table(fp_xtfs, dir_index_table, father_index_table_blocknr * BLOCK_SIZE);
        }
    }

    int file_index;
    file_index = find_dir_index_table(dir_names[dir_num], dir_index_table, type);
    if (file_index == NOT_FOUND) {
        printf("No such file %s with type %d!\n", argv[1], type);
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }

    // 处理目标目录
    refile_father_inode = get_root_inode(inode_table);
    refather_index_table_blocknr = inode_table[refile_father_inode].index_table_blocknr;
    read_dir_index_table(fp_xtfs, redir_index_table, refather_index_table_blocknr * BLOCK_SIZE);
    
    for (i = 0; i <= dir_renum; i++) {
        int child_in_father_index;
        child_in_father_index = find_dir_index_table(dir_renames[i], redir_index_table, DIR_FILE);
        // 找到和未找到的处理
        if (child_in_father_index == NOT_FOUND) {
            printf("No such dir %s to mv!\n", argv[3]);
            fclose(fp_xtfs);
            xtfs_exit(EXIT_FAILURE);
        } else {
            // 获取 inode 表项
            refile_father_inode = redir_index_table[child_in_father_index].pos;
            refather_index_table_blocknr = inode_table[refile_father_inode].index_table_blocknr;
            read_dir_index_table(fp_xtfs, redir_index_table, refather_index_table_blocknr * BLOCK_SIZE);
        }
    }

    // 待剪切的目录存在相同文件,不进行
    int refile_index;
    refile_index = find_dir_index_table(dir_names[dir_num], redir_index_table, type);
    if (refile_index != NOT_FOUND) {
        printf("The target address already has a file/folder named %s with type %d!\n", argv[1], type);
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }

    // 获取源文件 inode 表项，删除源文件的索引
    int file_inode = dir_index_table[file_index].pos;
    dir_index_table[file_index].pos = 0;
    dir_index_table[file_index].type = NO_FILE;

    // 在目标目录下进行剪切操作
    get_empty_dir_index(redir_index_table, dir_names[dir_num], type, file_inode);

    // 全部结束后写入文件系统  
    write_file(fp_xtfs, father_index_table_blocknr * BLOCK_SIZE, (char*)dir_index_table, CATALOG_TABLE_SIZE * sizeof(CATALOG));
    write_file(fp_xtfs, refather_index_table_blocknr * BLOCK_SIZE, (char*)redir_index_table, CATALOG_TABLE_SIZE * sizeof(CATALOG));

    // 将inode表写回0号数据块
    write_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);
    // 关闭文件系统
    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}