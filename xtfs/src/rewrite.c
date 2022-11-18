/**
 * @file rewrite.c
 * @author MDND
 * @brief 基础文件系统文件重写
 * @version 0.1
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xtfs_manage.h"
#include "xtfs_limits.h"
#include "xtfs_struct.h"
#include "xtfs_check.h"
#include "io.h"

#ifdef DEBUG
#include <assert.h>
#endif

// 读取的inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
// 数据块位图lowbit表
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
// 文件系统文件
char* fs_name = NULL;
// 文件系统文件索引
FILE* fp_xtfs = NULL;

int main(int argc, char* argv[]) {
    // INIT_XTFS_MANAGE
    FILE* fp = NULL;
    size_t filesize;
    char filename1[MAX_FILE_NAME_LENGTH] = {0};
    char filename2[MAX_FILE_NAME_LENGTH] = {0};
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    // blocknr：inode表位置，index_table_blocknr：数据块索引表位置
    int i, blocknr, index_table_blocknr, index_index_b = -1;
    size_t size;
    char buffer[BLOCK_SIZE];
    // 数据块内容已写进的数据块，更新后需要的数据块
    INDEX_TABLE_STRUC need, exist;
    // 数据块索引表已占用的数据块，更新后需要占用的数据块
    INDEX_TABLE_STRUC index_table_need, index_table_exist;

    check_file_name(argv[1]);
    check_file_name(argv[2]);
    check_fs_name(argv[3]);

    // 初始化lowbit表
    for (i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    // 取两个文件名称
    strcpy(filename1, argv[1]);
    strcpy(filename2, argv[2]);
    fs_name = argv[3];

    // 打开第二个待覆写文件，用于后续的数据读取
    fp = fopen(filename2, "r");
    filesize = read_file_size(fp);
    
    // fseek(fp, 0, SEEK_END);
    // filesize = ftell(fp);
    // fseek(fp, 0, SEEK_SET);

    // 打开文件系统
    fp_xtfs = fopen(fs_name, "r+");

    // 读取前两个数据块的数据
    read_first_two_blocks(fp_xtfs, inode_table, block_map);

    blocknr = find_inode_index_table(filename1, inode_table);

    if (blocknr == NOT_FOUND) {
        printf("No such file: %s\n", filename1);
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }

    // 取出第一个 index_table 的数据块地址
    index_table_blocknr = inode_table[blocknr].index_table_blocknr;
    // 取出第一个 index_table
    read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);

    // 数据块内容已经存储的数据块
    exist = (inode_table[blocknr].size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    // 数据块内容需要的数据块
    need = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    // 数据块索引表已经存储的数据块
    index_table_exist = (exist + INDEX_TABLE_DATA_SIZE - 1) / INDEX_TABLE_DATA_SIZE;
    // 数据块索引表需要的数据块
    index_table_need = (need + INDEX_TABLE_DATA_SIZE - 1) / INDEX_TABLE_DATA_SIZE;

    // 给数据块内容和数据块索引表分配新空间，若不足直接退出，不会再写下去
    INDEX_TABLE_STRUC* blocknr_s = NULL;
    INDEX_TABLE_STRUC* index_blocknr_s = NULL;
    blocknr_s = get_all_block(need - exist, block_map, lowbit);
    index_blocknr_s = get_all_block(index_table_need - index_table_exist, block_map, lowbit);

    // 能写尽写，将数据写入数据块
    for (i = 0; i < exist; i++) {
        memset(buffer, 0, BLOCK_SIZE);
        size = fread(buffer, 1, BLOCK_SIZE, fp);
        if (size == 0) {
            break;
        }
        // 检查 index_table ，得到应该写入的正确位置，若当前已满，需要先写入，再读出以存在的下一个文件系统进行读取
        INDEX_TABLE_STRUC index = i;
        if ((index + 1) % (INDEX_TABLE_DATA_SIZE + 1) == 0) {
            INDEX_TABLE_STRUC temp = index_table[INDEX_TABLE_DATA_SIZE];
            write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            index_table_blocknr = temp;
            memset(index_table, 0, INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
            read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            index = 0;
        } else {
            index = index % INDEX_TABLE_DATA_SIZE;
        }
        // 无论怎么样，都将一整块数据输入，防止之前的文件内容干扰。
        write_file(fp_xtfs, index_table[index] * BLOCK_SIZE, buffer, BLOCK_SIZE);
    }

    // 若还需要空间则继续输入
    for (i = 0; i < need - exist; i++) {
        // 下一个写入的块
        int next_blocknr = blocknr_s[i];
        // 读文件内容
        memset(buffer, 0, BLOCK_SIZE);
        size = fread(buffer, 1, BLOCK_SIZE, fp);
        if (size == 0) {
            break;
        }
        // 将文件内容写进下一个块
        write_file(fp_xtfs, next_blocknr * BLOCK_SIZE, buffer, BLOCK_SIZE);
        // 检查 index_table ，得到应该写入的正确位置，若当前已满，需要先写入，再读出不存在文件系统的下一个进行输入
        INDEX_TABLE_STRUC index = i + exist;
        if ((index + 1) % (INDEX_TABLE_DATA_SIZE + 1) == 0) {
            INDEX_TABLE_STRUC temp = index_blocknr_s[++index_index_b];
            index_table[INDEX_TABLE_DATA_SIZE] = temp;
            write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            index_table_blocknr = temp;
            memset(index_table, 0, INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
            index = 0;
            // 修改数据块位图（可能冗余）
            // set_block_map(1, , block_map);
        } else {
            index = index % INDEX_TABLE_DATA_SIZE;
        }
        // 修改index_table
        index_table[index] = next_blocknr;
        // 修改数据块位图（可能冗余）
        // set_block_map(1, next_blocknr, block_map);
    }

    // 释放不需要的文件位置
    for (i = need; i < exist; i++) {
        // 检查 index_table ，得到应该释放的正确位置，若当前已满，需要将其当前index_table最后的标识位置为0，更改 block_map 并写进，再读出以存在文件系统中的下一个数据块索引表进行释放
        INDEX_TABLE_STRUC index = i;
        if ((index + 1) % (INDEX_TABLE_DATA_SIZE + 1) == 0) {
            INDEX_TABLE_STRUC temp = index_table[INDEX_TABLE_DATA_SIZE];
            index_table[INDEX_TABLE_DATA_SIZE] = 0;
            write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            index_table_blocknr = temp;
            set_block_map(0, index_table_blocknr, block_map);
            memset(index_table, 0, INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
            read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            index = 0;
        } else {
            index = index % INDEX_TABLE_DATA_SIZE;
        }
        // 修改数据块位图
        set_block_map(0, index_table[index], block_map);
        // 修改数据块索引
        index_table[index] = 0;
    }

    // 将未被写入的 index_table 写入文件系统
    write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
    
    // 修改inode记录的文件大小
    inode_table[blocknr].size = filesize;
    // 修改inode记录的文件名称
    strcpy(inode_table[blocknr].filename, filename2);
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    xtfs_free_pool();
    fclose(fp);
    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}

