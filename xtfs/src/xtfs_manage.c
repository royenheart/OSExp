/**
 * @file xtfs_manage.c
 * @author RoyenHeart, ovovcast, MDND, bow
 * @brief 提供统一的内存管理、退出机制、错误信息管理机制，常用函数封装（实现）
 * @version 1.0.0
 * @date 2022-11-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "xtfs_manage.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "xtfs_limits.h"
#include "xtfs_struct.h"

#ifdef FEATURE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

void *pool[MAX_POOL_LENGTH];
int pool_top = -1;
specs_copy specs_params;

void xtfs_exit(int ret) {
    xtfs_free_pool();
    exit(ret);
}

void *xtfs_free_pool(void) {
    int i;
    for (i = 0; i < pool_top; i++) {
        free(pool[i]);
    }
}

void *xtfs_malloc(size_t size) {
    if (pool == NULL) {
        printf("XTFS_MANAGE NOT INIT!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    void *t = malloc(size);
    pool[++pool_top] = t;
    return t;
}

short get_block(BLOCK_MAP_STRUC *block_map, BLOCK_MAP_TABLE_STRUC *lowbit) {
    short blocknr;
    int i;

    // 遍历数据块位图
    for (i = 0; i < BLOCK_SIZE; i++) {
        // 当前块是否占满
        if (block_map[i] != 255) {
            // 遍历当前数据块位图的8个比特看是否有空余的数据块（512字节），使用lowbit方法
            int x = block_map[i];
            x = ~x;
            x = x & (-x);
            block_map[i] |= x;
            blocknr = (i << 3) + lowbit[x];
            return blocknr;
        }
    }
    printf("block_map is empty.\n");
    xtfs_exit(EXIT_FAILURE);
}

short *get_all_block(int need, BLOCK_MAP_STRUC *block_map,
                     BLOCK_MAP_TABLE_STRUC *lowbit) {
    // 必须返回 need + 1 空间，防止缓冲区溢出，但是申请的时候还是按照 need
    // 数量来
    if (need <= 0) {
        return memset(
            (INDEX_TABLE_STRUC *)xtfs_malloc(sizeof(INDEX_TABLE_STRUC)), 0,
            sizeof(INDEX_TABLE_STRUC));
    }
    INDEX_TABLE_STRUC *blocknr_s = (INDEX_TABLE_STRUC *)xtfs_malloc(
        (need + 1) * sizeof(INDEX_TABLE_STRUC));
    memset(blocknr_s, 0, (need + 1) * sizeof(INDEX_TABLE_STRUC));
    int i;
    for (i = 0; i < need; i++) {
        blocknr_s[i] = get_block(block_map, lowbit);
    }
    return blocknr_s;
}

void write_first_two_blocks(FILE *fp_xtfs, struct inode *inode_table,
                            BLOCK_MAP_STRUC *block_map) {
    write_file(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
    write_file(fp_xtfs, BLOCK_SIZE, (char *)block_map, BLOCK_SIZE);
}

void read_first_two_blocks(FILE *fp_xtfs, struct inode *inode_table,
                           BLOCK_MAP_STRUC *block_map) {
    read_file(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
    read_file(fp_xtfs, BLOCK_SIZE, block_map, BLOCK_SIZE);
}

short write_index_table(FILE *fp_xtfs, BLOCK_MAP_STRUC *block_map,
                        BLOCK_MAP_TABLE_STRUC *lowbit,
                        INDEX_TABLE_STRUC *index_table) {
    short index_table_blocknr;

    index_table_blocknr = get_block(block_map, lowbit);
    write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char *)index_table,
               BLOCK_SIZE);
    return index_table_blocknr;
}

void read_index_table(FILE *fp_xtfs, INDEX_TABLE_STRUC *index_table,
                      int offset) {
    memset(index_table, 0, INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
    read_file(fp_xtfs, offset, (char *)index_table,
              INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
}

void read_dir_index_table(FILE *fp_xtfs, CATALOG *index_table, int offset) {
    memset(index_table, 0, CATALOG_TABLE_SIZE * sizeof(CATALOG));
    read_file(fp_xtfs, offset, (char *)index_table,
              CATALOG_TABLE_SIZE * sizeof(CATALOG));
}

int select_spec_funcs(char *buffer, int file_type) {
    int ret = 0;
    if (is_spec_format(file_type, CIPHER) == 0) {
        ret |= spec_cipher_copy(buffer);
    } else if (is_spec_format(file_type, ZIP) == 0) {
        ret |= spec_huffman_zip_copy(buffer);
    } else {
        ret |= normal_copy(buffer);
    }
    return ret;
}

short copy_blocks(int file_size, int file_type, BLOCK_MAP_STRUC *block_map,
                  BLOCK_MAP_TABLE_STRUC *lowbit, FILE *fp_xtfs) {
    int i, need, index_need;
    // index_table 需要的数据块的位置
    INDEX_TABLE_STRUC *index_blocknr_s = NULL;
    // 数据内容需要的数据块的位置
    INDEX_TABLE_STRUC *blocknr_s = NULL;
    // 当前需要写入数据内容的数据块位置
    INDEX_TABLE_STRUC blocknr;
    // 当前需要写入 index_table 的数据块的位置
    INDEX_TABLE_STRUC index_blocknr = 0;
    // 当前index_table的位置
    INDEX_TABLE_STRUC index_index_b = 0;
    char buffer[BLOCK_SIZE];
    INDEX_TABLE_STRUC index_block[INDEX_TABLE_SIZE];

    // 计算存储数据内容需要的数据块和存储数据块索引表需要的数据块
    need = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    index_need = (need + INDEX_TABLE_DATA_SIZE - 1) / INDEX_TABLE_DATA_SIZE;

    // 申请全部需要的空间，若空间不够将直接退出，不进行之后的保存
    blocknr_s = get_all_block(need, block_map, lowbit);
    index_blocknr_s = get_all_block(index_need, block_map, lowbit);

    memset(index_block, 0, INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
    for (i = 0; i < need; i++) {
        blocknr = blocknr_s[i];
        memset(buffer, 0, BLOCK_SIZE);
        // 对需要输入文件系统分区的字节数组进行数据的读取和处理
        select_spec_funcs(buffer, file_type);
        // 将数据内容写入文件系统分区
        write_file(fp_xtfs, blocknr * BLOCK_SIZE, buffer, BLOCK_SIZE);

        index_block[index_index_b] = blocknr;
        index_index_b++;
        // 当前数据块索引表已满，写入文件系统分区并切换至下一个
        if (index_index_b == INDEX_TABLE_DATA_SIZE) {
            INDEX_TABLE_STRUC index = index_blocknr_s[index_blocknr];
            INDEX_TABLE_STRUC index_next = index_blocknr_s[index_blocknr + 1];
            index_block[index_index_b] = index_next;
            write_file(fp_xtfs, index * BLOCK_SIZE, (char *)index_block,
                       BLOCK_SIZE);
            memset(index_block, 0,
                   INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
            index_index_b = 0;
            index_blocknr++;
        }
    }

    // 将剩余未被写入的 index_table 写入文件系统分区
    if (index_blocknr_s[index_blocknr] != 0) {
        INDEX_TABLE_STRUC index = index_blocknr_s[index_blocknr];
        write_file(fp_xtfs, index * BLOCK_SIZE, (char *)index_block,
                   BLOCK_SIZE);
    }

    // 全部写入完成，退出并返回第一个 index_table 所处的位置
    return index_blocknr_s[0];
}

int normal_params_load(FILE *file) {
    specs_params.normal_params.file = file;
    return 0;
}

int normal_copy(char *buffer) {
    FILE *fp = specs_params.normal_params.file;
    fread(buffer, 1, BLOCK_SIZE, fp);
    return 0;
}

int spec_cipher_params_load(unsigned int pwd, FILE *file) {
    specs_params.cipher_params.pwd = pwd;
    specs_params.cipher_params.file = file;
    return 0;
}

int spec_cipher_copy(char *buffer) {
    unsigned int pwd = specs_params.cipher_params.pwd;
    int j;
    FILE *fp = specs_params.cipher_params.file;
    fread(buffer, 1, BLOCK_SIZE, fp);
    for (j = 0; j < BLOCK_SIZE; j++) {
        buffer[j] ^= pwd;
    }
    return 0;
}

int spec_huffman_zip_params_load(int file_size, int *pos, const char *file) {
    specs_params.huffman_params.file_size = file_size;
    specs_params.huffman_params.pos = pos;
    specs_params.huffman_params.file = file;
    return 0;
}

int spec_huffman_zip_copy(char *buffer) {
    int j;
    int file_size = specs_params.huffman_params.file_size;
    int *pos = specs_params.huffman_params.pos;
    const char *file = specs_params.huffman_params.file;
    size_t size =
        (BLOCK_SIZE < file_size - *pos) ? BLOCK_SIZE : file_size - *pos;
    for (j = *pos; j < *pos + size; j++) {
        buffer[j - *pos] = file[j];
    }
    *pos += size;
    return 0;
}

void set_block_map(int flag, short blocknr, BLOCK_MAP_STRUC *block_map) {
    int j = blocknr % 8;
    int i = (blocknr - j) / 8;
    if (flag == 1) {
        block_map[i] |= 1 << j;
    } else {
        block_map[i] &= ~(1 << j);
    }
}

int find_inode_table(char *filename, struct inode *inode_table) {
    int i;
    for (i = 0; i < NR_INODE; i++) {
        if (inode_table[i].type == NO_FILE) {
            continue;
        }
        if (strcmp(inode_table[i].filename, filename) == 0) {
            return i;
        }
    }
    return NOT_FOUND;
}

int find_dir_index_table(char *filename, CATALOG *index_table, int type) {
    int i;
    for (i = 0; i < CATALOG_TABLE_DATA_SIZE; i++) {
        if (strcmp(index_table[i].filename, filename) == 0 &&
            index_table[i].type == type) {
            return i;
        }
    }
    if (i == CATALOG_TABLE_DATA_SIZE) {
        return NOT_FOUND;
    }
}

int get_root_inode(struct inode *inode_table) {
    int i;
    for (i = 0; i < NR_INODE; i++) {
        if (strcmp(inode_table[i].filename, "/") == 0 &&
            inode_table[i].type == DIR_FILE) {
            return i;
        }
    }
    return NOT_FOUND;
}

int get_empty_inode(struct inode *inode_table, char *filename, int type) {
    int i;

    // 遍历inode表
    for (i = 0; i < NR_INODE; i++) {
        if (inode_table[i].type == NO_FILE) {
            // inode表只有一种数据结构进行操作，且类型是判断是否存在文件的标志，不须担心之前的数据影响
            inode_table[i].type = type;
            strncpy(inode_table[i].filename, filename, MAX_FILE_NAME_LENGTH);
            break;
        }
    }

    if (i == NR_INODE) {
        printf("inode_table is empty.\n");
        xtfs_exit(EXIT_FAILURE);
    }

    return i;
}

int get_empty_dir_index(CATALOG *index_table, char *filename, int type,
                        int pos) {
    int i;
    int found = 0;
    int ret;

    // 遍历 index_table
    for (i = 0; i < CATALOG_TABLE_DATA_SIZE; i++) {
        if (!found && index_table[i].type == NO_FILE) {
            ret = i;
            found = 1;
        } else if (is_same_type_class(index_table[i].type, type) &&
                   strcmp(index_table[i].filename, filename) == 0) {
            // 可能需要执行覆写？
            printf("File Exits!.\n");
            xtfs_exit(EXIT_FAILURE);
        }
    }

    if (i == NR_INODE) {
        printf("index_table is empty.\n");
        xtfs_exit(EXIT_FAILURE);
    }

    index_table[ret].type = type;
    strncpy(index_table[ret].filename, filename, MAX_FILE_NAME_LENGTH);
    index_table[ret].pos = pos;

    return ret;
}