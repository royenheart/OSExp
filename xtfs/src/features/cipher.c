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
char* fs_name = NULL;
FILE* fp_xtfs = NULL;
unsigned int pwd = 0;

// int copy_blocks(char* filename, short* index_table) {
//     FILE* fp = NULL;
//     size_t filesize;
//     int i, need;
//     int blocknr;
//     char buffer[BLOCK_SIZE] = {0};

//     fp = fopen(filename, "r");
//     filesize = read_file_size(fp);
//     // fseek(fp, 0, SEEK_END);
//     // filesize = ftell(fp);
//     // fseek(fp, 0, SEEK_SET);
//     memset((char*)index_table, 0, BLOCK_SIZE);
//     need = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;
//     short* blocknr_s = get_all_block(need, block_map, lowbit);
//     for (i = 0; i < need; i++) {
//         blocknr = blocknr_s[i];
//         memset(buffer, 0, BLOCK_SIZE);
//         fread(buffer, 1, BLOCK_SIZE, fp);
//         for (int j = 0; j < BLOCK_SIZE; j++) {
//             buffer[j] ^= pwd;
//         }
//         write_file(fp_xtfs, blocknr * BLOCK_SIZE, buffer, BLOCK_SIZE);
//         index_table[i] = blocknr;
//     }
//     fclose(fp);
//     return filesize;
// }

int main(int argc, char* argv[]) {
    // INIT_XTFS_MANAGE
    size_t filesize;
    short index_table_blocknr;
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    char* filename = NULL;
    char* password = NULL;
    char type;
    FILE* fp = NULL;
    int inode_nr;
    int i;

    for (i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    check_file_name(argv[1]);
    check_fs_name(argv[3]);

    filename = argv[1];
    type = get_file_type(atoi(argv[2]) | CIPHER);
    fs_name = argv[3];
    password = argv[4];
    pwd = 0;

    fp_xtfs = fopen(fs_name, "r+");

    // 生成密码
    for (int i = 0; i < strlen(password); i++) {
        pwd = pwd * 10 + (password[i] - '0');
    }

    read_first_two_blocks(fp_xtfs, inode_table, block_map);
    fp = fopen(filename, "r");
    filesize = read_file_size(fp);
    spec_cipher_params_load(pwd, fp);
    index_table_blocknr = copy_blocks(filesize, type, block_map, lowbit, fp_xtfs);
    fclose(fp);
    // filesize = copy_blocks(filename, index_table);
    // index_table_blocknr = write_index_table(fp_xtfs, block_map, lowbit, index_table);
    inode_nr = get_empty_inode(inode_table, filename, type);
    inode_table[inode_nr].size = filesize;
    inode_table[inode_nr].index_table_blocknr = index_table_blocknr;
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}
