/**
 * @file cipher.c
 * @author MDND
 * @brief XTFS 文件系统分区文件加密
 * @version 1.0.0
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../xtfs_check.h"
#include "../xtfs_limits.h"
#include "../xtfs_struct.h"
#include "../xtfs_manage.h"
#include "../lex/folder_lex.h"
#include "../io.h"

struct inode inode_table[NR_INODE];
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
FILE* fp_xtfs = NULL;
unsigned int pwd = 0;

int main(int argc, char* argv[]) {
    char** dirnames = NULL;
    int dir_num;
    int i;
    // 目录所需数据
    CATALOG dir_index_table[CATALOG_TABLE_SIZE];
    // 文件所需数据
    int inode_blocknr;
    size_t filesize;
    int type;
    char* password = NULL;
    INDEX_TABLE_STRUC index_table_blocknr;

    for (i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    check_fs_name(argv[3]);

    dir_num = get_folders(argv[1], &dirnames);

    if (dir_num == ERROR_PARSE) {
        printf("cipher failed, check the input file format!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    type = get_file_type(atoi(argv[2]) | CIPHER);
    if (is_same_type_class(type, DIR_FILE) == 1) {
        printf("Can't cipher all folder right now!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    strncpy(fs_name, argv[3], MAX_FS_NAME_LENGTH);
    password = argv[4];
    pwd = 0;

    fp_xtfs = fopen(fs_name, "r+");

    // 生成密码
    for (int i = 0; i < strlen(password); i++) {
        pwd = pwd * 10 + (password[i] - '0');
    }

    read_first_two_blocks(fp_xtfs, inode_table, block_map);

    inode_blocknr = get_root_inode(inode_table);
    if (inode_blocknr == NOT_FOUND) {
        printf("Root has been destroyed! This file system may not be in secure state!\n");
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }
    index_table_blocknr = inode_table[inode_blocknr].index_table_blocknr;
    read_dir_index_table(fp_xtfs, dir_index_table, index_table_blocknr * BLOCK_SIZE);

    for (i = 0; i < dir_num; i++) {
        int child_in_father_index;
        child_in_father_index = find_dir_index_table(dirnames[i], dir_index_table, DIR_FILE);
        if (child_in_father_index == NOT_FOUND) {
            printf("No such dir!\n");
            fclose(fp_xtfs);
            xtfs_exit(EXIT_FAILURE);
        } else {
            int curr_inode = dir_index_table[child_in_father_index].pos;
            index_table_blocknr = inode_table[curr_inode].index_table_blocknr;
            read_dir_index_table(fp_xtfs, dir_index_table, index_table_blocknr * BLOCK_SIZE);
            inode_blocknr = curr_inode;
        }
    }

    // 在inode表中申请一个空闲inode，存放文件的inode信息
    inode_blocknr = get_empty_inode(inode_table, dirnames[dir_num], type);
    // 在目录中添加对应的表项，并重新写回文件系统分区
    get_empty_dir_index(dir_index_table, dirnames[dir_num], type, inode_blocknr);
    write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)dir_index_table, CATALOG_TABLE_SIZE * sizeof(CATALOG));

    FILE* fp = fopen(argv[1], "r");
    filesize = read_file_size(fp);
    spec_cipher_params_load(pwd, fp);
    index_table_blocknr = copy_blocks(filesize, type, block_map, lowbit, fp_xtfs);
    fclose(fp);

    inode_table[inode_blocknr].size = filesize;
    inode_table[inode_blocknr].index_table_blocknr = index_table_blocknr;
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}
