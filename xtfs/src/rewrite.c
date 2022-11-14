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

#ifdef DEBUG
#include <assert.h>
#endif

int find_inode_index_table(char *filename, char type);
void set_block_map(int flag, short blocknr);
void read_first_two_blocks();
void write_first_two_blocks();

// 读取的inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
unsigned char block_map[BLOCK_SIZE];
// 文件系统文件
char *fs_name = NULL;
// 文件系统文件索引
FILE *fp_xtfs = NULL;

/**
 * @brief 读取inode表和数据块视图
 * 
 */
void read_first_two_blocks()
{
	read_file(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
	read_file(fp_xtfs, BLOCK_SIZE, block_map, BLOCK_SIZE);
}

void set_block_map(int flag, short blocknr)
{
    int j = blocknr % 8;
    int i = (blocknr-j) / 8;
    if(flag == 1)
    {
        block_map[i] |= 1<<j;
    }
    else
    {
        block_map[i] &= ~(1<<j);
    }
}

void write_first_two_blocks()
{
	write_file(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
	write_file(fp_xtfs, BLOCK_SIZE, block_map, BLOCK_SIZE);
}

short get_block()
{
    short blocknr;
    int i, j;

    for (i = 0; i < BLOCK_SIZE; i++)
    {
        if (block_map[i] == 255)
            continue;
        for (j = 0; j < 8; j++)
        {
            if ((block_map[i] & (1 << j)) != 0)
                continue;
            block_map[i] |= 1 << j;
            blocknr = i * 8 + j;
            return blocknr;
        }
    }
    printf("block_map is empty.\n");
    exit(EXIT_FAILURE);
}

/**
 * @brief 根据需要的块数获取所有可行的数据块编号
 * 
 * @param need 需要的数据块数
 * @return short* 数据块编号数组
 */
short* get_all_block(int need) {
	short *blocknr_s = (short*)xtfs_malloc(BLOCK_SIZE * sizeof(short));
	memset(blocknr_s, 0, BLOCK_SIZE * sizeof(short));
	int i;
	for (i = 0; i < need; i++) {
		blocknr_s[i] = get_block();
	}
	return blocknr_s;
}

short write_index_table(char *index_table)
{
	short index_table_blocknr;

	index_table_blocknr = get_block();
	write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char *)index_table, BLOCK_SIZE);
	return index_table_blocknr;
}

int main(int argc, char *argv[])
{
    INIT_XTFS_MANAGE
    
    FILE *fp = NULL;
    int filesize;
    char filename1[MAX_FILE_NAME_LENGTH] = {0};
    char filename2[MAX_FILE_NAME_LENGTH] = {0};
    short index_table[BLOCK_SIZE / 2] = {0};
    // blocknr：inode表位置，index_table_blocknr：数据块索引表位置
    int i, blocknr, index_table_blocknr;
    size_t size;
    char buffer[BLOCK_SIZE];
    short need, exist;

    check_file_name(argv[1]);
    check_file_name(argv[2]);
    check_fs_name(argv[3]);

    // 取两个文件名称
    strcpy(filename1, argv[1]);
    strcpy(filename2, argv[2]);

    // 打开第二个待覆写文件，用于后续的数据读取
    fp = fopen(filename2, "r");
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // 取文件系统名，并打开文件系统
    fs_name = argv[3];
    fp_xtfs = fopen(fs_name, "r+");

    // 读取前两个数据块的数据
    read_first_two_blocks();

    for (i = 0; i < NR_INODE; i++) {
        // 未加入目录，目前只以文件名判断且不是空文件（文件类型）
        // 覆写必须要原文件存在
        if (strcmp(inode_table[i].filename, filename1) == 0 && inode_table[i].type != NO_FILE) {
            blocknr = i;
            break;
        }
    }

    if (i == NR_INODE) {
        printf("No such file: %s\n", filename1);
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }

    // 取出index_table存的地址
    index_table_blocknr = inode_table[blocknr].index_table_blocknr; 
    // 取出index_table
    // read_index_block(index_table, index_table_blocknr);   
    read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);

    // 取已经存的数据块
    exist = (inode_table[blocknr].size + BLOCK_SIZE - 1) / BLOCK_SIZE; 
    // 计算需要的数据块
    need = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;                  

    // 分配新空间
    short *blocknr_s = NULL;
    blocknr_s = get_all_block(need - exist);

    // 能写尽写，将数据写入数据块
    for (i = 0; i < exist; i++) 
    {
        memset(buffer, 0, BLOCK_SIZE);
        size = fread(buffer, 1, BLOCK_SIZE, fp);
        if (size == 0)
        {
            break;
        }
        // 无论怎么样，都将一整块数据输入，防止最后一块之前的文件内容干扰。
        write_file(fp_xtfs, index_table[i] * BLOCK_SIZE, buffer, BLOCK_SIZE);
    }

    // 若还需要文件则继续输入
    for (i = 0; i < need - exist; i++) {
        // 下一个写入的块
        int next_blocknr = blocknr_s[i];
        // 读文件内容
        memset(buffer, 0, BLOCK_SIZE);
        size = fread(buffer, 1, BLOCK_SIZE, fp); 
        if (size == 0)
        {
            break;
        }
        // 将文件内容写进下一个块
        write_file(fp_xtfs, next_blocknr * BLOCK_SIZE, buffer, BLOCK_SIZE);
        // 修改index_table
        index_table[i + exist] = next_blocknr;
        // 修改数据块位图
        set_block_map(1, next_blocknr);
    }
    // 释放不需要的文件位置
    for (i = need; i < exist; i++) {
        // 修改数据块位图
        set_block_map(0, index_table[i]); 
        // 修改数据块索引，覆写文件仍然存在
        index_table[i] = 0;
    }

    // 将更新后的数据块索引表 index_table 写入文件系统
    // 修改inode记录的文件大小
    inode_table[blocknr].size = filesize;
    // 修改inode记录的文件名称 
    strcpy(inode_table[blocknr].filename, filename2);
    write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char *)index_table, BLOCK_SIZE); 
    write_first_two_blocks();

    xtfs_free_pool();
    fclose(fp);
    fclose(fp_xtfs);
    return(EXIT_SUCCESS);
}

