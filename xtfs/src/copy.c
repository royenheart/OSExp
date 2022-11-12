/**
 * @file copy.c
 * @author RoyenHeart, Ruoxuan Wang, MDND, bow
 * @brief 基础文件系统文件拷贝
 * @version 0.1
 * @date 2022-10-25
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
#include "io.h"

// 读取的inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
// 数据块位图lowbit表
char lowbit[BLOCK_MAP_TABLE_SIZE];
// 文件系统文件名
char *fs_name = NULL;
// 文件系统文件索引
FILE *fp_xtfs = NULL;

/**
 * @brief 获取空余数据块
 *
 * @return short blocknr 返回在数据块位图的第几个字节的第几位（表示具体哪一块，512 * 8 = 4096）
 */
short get_block()
{
	short blocknr;

	// 遍历数据块位图
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
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

/**
 * @brief 读取inode表和数据块视图
 * 
 */
void read_first_two_blocks()
{
	fp_xtfs = fopen(fs_name, "r+");
	read_file(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
	read_file(fp_xtfs, BLOCK_SIZE, block_map, BLOCK_SIZE);
}

/**
 * @brief 打开文件并将内容拷贝到数据块中，调用get_block和write_block
 *
 * @param filename 文件名
 * @param index_table 文件数据块索引表
 * @return int filesize 拷贝的文件大小
 */
int copy_blocks(char *filename, short *index_table)
{
	FILE *fp = NULL;
	int filesize;
	int i, need;
	int blocknr;
	size_t size;
	char buffer[BLOCK_SIZE];

	fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	memset((char *)index_table, 0, BLOCK_SIZE);
	// 将整个文件读入到文件系统中，并更新其数据块索引；
	// 先获取所有可行块，避免空间不够导致的额外开销
	need = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;
	short *blocknr_s = get_all_block(need);
	for (i = 0; i < need; i++)
	{
		blocknr = blocknr_s[i];
		// 读取文件内容，以每个元素1个字节读入到512大小的字节数组buffer中
		// 后续读取会以上次读取停留的指针继续
		size = fread(buffer, 1, BLOCK_SIZE, fp);
		write_file(fp_xtfs, blocknr * BLOCK_SIZE, buffer, size);
		index_table[i] = blocknr;
	}
	fclose(fp);
	return filesize;
}

/**
 * @brief 写入文件对应的数据块索引表
 * 
 * @param index_table 数据块索引表
 * @return short index_table_blocknr 存储数据块索引表的块号
 */
short write_index_table(char *index_table)
{
	short index_table_blocknr;

	index_table_blocknr = get_block();
	write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char *)index_table, BLOCK_SIZE);
	return index_table_blocknr;
}

/**
 * @brief 将文件inode数据写入inode表
 * 
 * @param filename 文件名
 * @param filesize 文件大小
 * @param index_table_blocknr 文件数据块索引表所在数据块块号 
 * @param type 文件类型
 */
void get_empty_inode(char *filename, int filesize, short index_table_blocknr, char type)
{
	int i;

	// 遍历inode表
	for (i = 0; i < NR_INODE; i++)
	{
		if (!inode_table[i].type)
		{
			inode_table[i].size = filesize;
			inode_table[i].type = type;
			inode_table[i].index_table_blocknr = index_table_blocknr;
			strcpy(inode_table[i].filename, filename);
			break;
		}
	}

	if (i == NR_INODE)
	{
		printf("inode_table is empty.\n");
		xtfs_exit(EXIT_FAILURE);
	}
}

/**
 * @brief 将更新的inode表和数据块视图写回文件系统
 * 
 */
void write_first_two_blocks()
{
	write_file(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
	write_file(fp_xtfs, BLOCK_SIZE, block_map, BLOCK_SIZE);
	fclose(fp_xtfs);
}

/**
 * @brief copy
 * 
 * @param argc 参数个数
 * @param argv 参数列表
 * @return int 文件拷贝完成状态
 */
int main(int argc, char *argv[])
{
	INIT_XTFS_MANAGE
	int filesize;
	short index_table_blocknr;
	// 文件的数据块索引表
	// 数据块索引表用于记录文件在此文件系统中占用了哪些数据块，存放该文件占用的所有数据块的块号
	// 块号即i * 8 + j
	short index_table[BLOCK_SIZE / 2];
	char *filename;
	char type;
	int i;

	// 初始化lowbit表
	for (i = 0; i < 8; i++) {
		lowbit[1 << i] = i;
	}

	// 获取待拷贝文件名和文件类型
	filename = argv[1];
	type = atoi(argv[2]);
	fs_name = argv[3];
	// 读取0号和1号inode表和数据块位图数据到进程管理的内存（数组），便于修改
	// 全局变量：inode_table 和 block_map
	read_first_two_blocks();
	// 将文件中内容拷贝到xtfs文件系统中
	filesize = copy_blocks(filename, index_table);
	// 将数据块索引表拷贝到xtfs文件系统中
	index_table_blocknr = write_index_table((char *)index_table);
	// 在inode表中申请一个空闲inode，存放文件的inode信息
	get_empty_inode(filename, filesize, index_table_blocknr, type);
	// 将0号和1号数据块内容写回并关闭文件系统文件
	write_first_two_blocks();

	return(EXIT_SUCCESS);
}