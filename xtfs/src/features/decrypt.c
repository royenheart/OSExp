/**
 * @file decrypt.c
 * @author MDND
 * @brief 解密
 * @version 0.1
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../xtfs_limits.h"
#include "../xtfs_struct.h"
#include "../xtfs_manage.h"
#include "../xtfs_check.h"
#include "../io.h"

#ifdef DEBUG
#include <assert.h>
#endif

struct inode inode_table[NR_INODE];
char* fs_name = NULL;
FILE* fp_xtfs = NULL;
unsigned int pwd;

int main(int argc, char* argv[]) {
    char filename[MAX_FILE_NAME_LENGTH] = {0};
    char* password = NULL;
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    int i;

    check_file_name(argv[1]);
    check_fs_name(argv[2]);

    strcpy(filename, argv[1]);
    fs_name = argv[2];
    fp_xtfs = fopen(fs_name, "r");

    password = argv[3];
    pwd = 0;
    for (int i = 0; i < strlen(password); i++) {
        pwd = pwd * 10 + (password[i] - '0');
    }

#ifdef DEBUG
    assert(fp_xtfs != NULL);
#endif

    read_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);

    for (i = 0; i < NR_INODE; i++) {
        if (strcmp(inode_table[i].filename, filename) == 0) {
            long int blocknr = inode_table[i].index_table_blocknr;
            read_file(fp_xtfs, blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            break;
        }
    }

    if (i == NR_INODE) {
        printf("No such file: %s\n", filename);
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }

    for (i = 0; i < BLOCK_SIZE / 2; i++) {
        char data[BLOCK_SIZE + 1] = {0};
        int block_pos = index_table[i];
        if (block_pos == 0) {
            break;
        }
        read_file(fp_xtfs, block_pos * BLOCK_SIZE, data, BLOCK_SIZE);
        for (int i = 0; i < BLOCK_SIZE; i++) {
            data[i] = data[i] ^ pwd;
        }
        printf("%s", data);
    }

    fclose(fp_xtfs);
    return (EXIT_SUCCESS);
}
