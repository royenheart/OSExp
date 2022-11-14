/**
 * @file delete.c
 * @author bow
 * @brief xtfs文件系统文件删除
 * @version 0.1
 * @date 2022-11-14
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

// 读取的inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
unsigned char block_map[BLOCK_SIZE];
// 文件系统文件索引
int find_inode_index_table(char *filename);
void set_block_map(int flag, short blocknr);
void read_first_two_blocks();
void write_first_two_blocks();

char *fs_name;
FILE *fp_xtfs = NULL;

int find_inode_index_table(char *filename)
{
    // 遍历inode_table数组，找到与文件名对应的inode
    int i;
    for(i = 0; i < NR_INODE; i++)
    {
        if (inode_table[i].type == NO_FILE) {
            continue;
        }
        if (strcmp(inode_table[i].filename, filename) == 0)
        {
            return i;
        }
    }
    return EXIT_FAILURE;
}

void set_block_map(int flag, short blocknr)
{
    int j = blocknr % 8;
    int i = (blocknr - j) / 8;
    if(flag == 1)
    {
        block_map[i] |= 1<<j;
    }
    else
    {
        block_map[i] &= ~(1<<j);
    }
}

void read_first_two_blocks()
{
	read_file(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
	read_file(fp_xtfs, BLOCK_SIZE, block_map, BLOCK_SIZE);
}

void write_first_two_blocks()
{
	write_file(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
	write_file(fp_xtfs, BLOCK_SIZE, block_map, BLOCK_SIZE);
}

int main(int argc, char *argv[]) {
	short index_table_blocknr;
	short index_table[BLOCK_SIZE / 2];
	char *filename;
    int i;

    check_file_name(argv[1]);
    check_fs_name(argv[2]);

    // 读取0号和1号inode表和数据块位图数据到进程管理的内存（数组），便于修改
    // 全局变量：inode_table 和 block_map
    // 获取文件名
	filename = argv[1]; 
    fs_name = argv[2];

    fp_xtfs = fopen(fs_name, "r+");

    // 加载前两个数据块到 inode_table 和 block_map数组中
    read_first_two_blocks();

    int inode_index_table = -1;
    inode_index_table = find_inode_index_table(filename);
    if(inode_index_table == -1)
    {
        printf("the file does not exist!\n"); // 返回-1表示文件不存在
        return EXIT_FAILURE;
    }
    // 将数据块索引表中占用的数据块块号赋值
    index_table_blocknr = inode_table[inode_index_table].index_table_blocknr; 
    // 得到所有占用的数据块
    read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
    // 将数据块索引表和占用的数据块在数据块位图中置0
    set_block_map(0, index_table_blocknr);
    for(i = 0; i * BLOCK_SIZE <= inode_table[inode_index_table].size; i++)
    {
        // 将所有占用的数据块在block_map中置0
        set_block_map(0, index_table[i]); 
    }
    // 将对应 inode 表类型置0，即表示为空文件
    inode_table[inode_index_table].type = NO_FILE;
    // 将inode_table和block_map写回0/1号数据块
    write_first_two_blocks();
    fclose(fp_xtfs);
    return EXIT_SUCCESS;
}
