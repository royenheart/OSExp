/**
 * @file cipher.c
 * @author MDND
 * @brief 文件加密
 * @version 0.1
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
#include "../io.h"

struct inode inode_table[NR_INODE];
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
FILE* fp_xtfs = NULL;
unsigned int pwd = 0;

int main(int argc, char* argv[]) {
    // INIT_XTFS_MANAGE
    size_t filesize;
    short index_table_blocknr;
    char filename[MAX_FILE_NAME_LENGTH + 1] = {0};
    char* password = NULL;
    int type;
    FILE* fp = NULL;
    int inode_nr;
    int i;

    for (i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    check_file_name(argv[1]);
    check_fs_name(argv[3]);

    strncpy(filename, argv[1], MAX_FILE_NAME_LENGTH);
    type = get_file_type(atoi(argv[2]) | CIPHER);
    strncpy(fs_name, argv[3], MAX_FS_NAME_LENGTH);
    password = argv[4];
    pwd = 0;

    fp_xtfs = fopen(fs_name, "r+");

    // 生成密码
    for (int i = 0; i < strlen(password); i++) {
        pwd = pwd * 10 + (password[i] - '0');
    }

    read_first_two_blocks(fp_xtfs, inode_table, block_map);
    // 先申请一块空闲的 inode 表
    inode_nr = get_empty_inode(inode_table, filename, type);
    fp = fopen(filename, "r");
    filesize = read_file_size(fp);
    spec_cipher_params_load(pwd, fp);
    index_table_blocknr = copy_blocks(filesize, type, block_map, lowbit, fp_xtfs);
    fclose(fp);
    // filesize = copy_blocks(filename, index_table);
    // index_table_blocknr = write_index_table(fp_xtfs, block_map, lowbit, index_table);
    inode_table[inode_nr].size = filesize;
    inode_table[inode_nr].index_table_blocknr = index_table_blocknr;
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}
