/**
 * @file xtfs_manage.c
 * @author
 * @brief 提供统一的内存管理、退出机制、错误信息管理机制，常用函数封装（实现）
 * @version 0.1
 * @date 2022-11-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "xtfs_manage.h"
#include "xtfs_struct.h"

#ifdef FEATURE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

void** pool = NULL;
int pool_top = -1;

void xtfs_exit(int ret) {
    xtfs_free_pool();
    exit(ret);
}

void* xtfs_free_pool(void) {
    int i;
    for (i = 0; i < pool_top; i++) {
        free(pool[i]);
    }
}

void* xtfs_malloc(size_t size) {
#ifdef DEBUG
    assert(pool);
#endif
    if (pool == NULL) {
        printf("XTFS_MANAGE NOT INIT!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    void* t = malloc(size);
    pool[++pool_top] = t;
    return t;
}

short get_block(BLOCK_MAP_STRUC* block_map, BLOCK_MAP_TABLE_STRUC* lowbit) {
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

short* get_all_block(int need, BLOCK_MAP_STRUC* block_map, BLOCK_MAP_TABLE_STRUC* lowbit) {
    short* blocknr_s = (short*)xtfs_malloc(BLOCK_SIZE * sizeof(short));
    memset(blocknr_s, 0, BLOCK_SIZE * sizeof(short));
    int i;
    for (i = 0; i < need; i++) {
        blocknr_s[i] = get_block(block_map, lowbit);
    }
    return blocknr_s;
}

void write_first_two_blocks(FILE* fp_xtfs, struct inode* inode_table, BLOCK_MAP_STRUC* block_map) {
    write_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);
    write_file(fp_xtfs, BLOCK_SIZE, (char*)block_map, BLOCK_SIZE);
}

void read_first_two_blocks(FILE* fp_xtfs, struct inode* inode_table, BLOCK_MAP_STRUC* block_map) {
    read_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);
    read_file(fp_xtfs, BLOCK_SIZE, block_map, BLOCK_SIZE);
}

short write_index_table(FILE* fp_xtfs, BLOCK_MAP_STRUC* block_map, BLOCK_MAP_TABLE_STRUC* lowbit, INDEX_TABLE_STRUC* index_table) {
    short index_table_blocknr;

    index_table_blocknr = get_block(block_map, lowbit);
    write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
    return index_table_blocknr;
}

void set_block_map(int flag, short blocknr, BLOCK_MAP_STRUC* block_map) {
    int j = blocknr % 8;
    int i = (blocknr - j) / 8;
    if (flag == 1) {
        block_map[i] |= 1 << j;
    } else {
        block_map[i] &= ~(1 << j);
    }
}

int find_inode_index_table(char* filename, struct inode* inode_table) {
    // 遍历inode_table数组，找到与文件名对应的inode
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

int get_empty_inode(struct inode* inode_table, char* filename, char type) {
    int i;

    // 遍历inode表
    for (i = 0; i < NR_INODE; i++) {
        if (inode_table[i].type == NO_FILE) {
            inode_table[i].type = type;
            strcpy(inode_table[i].filename, filename);
            break;
        } else if (strcmp(inode_table[i].filename, filename) == 0) {
            // 需要执行覆写程序
            printf("File Exits!.\n");
            xtfs_exit(EXIT_FAILURE);
        }
    }

    if (i == NR_INODE) {
        printf("inode_table is empty.\n");
        xtfs_exit(EXIT_FAILURE);
    }

    return i;
}