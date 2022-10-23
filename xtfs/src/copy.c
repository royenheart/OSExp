#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xtfs_limits.h"
#include "xtfs_struct.h"

/**
 * @brief 文件拷贝
 *
 */

// 读取的inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
char block_map[BLOCK_SIZE];
// 文件系统文件名
char *fs_name;
// 文件系统文件索引
FILE *fp_xtfs;

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
		// 表示当前块已经占满
		if (block_map[i] == 255) {
			continue;
		}
		// 遍历当前数据块位图的8个比特看是否有空余的数据块（512字节）
		for (int j = 0; j < 8; j++)
		{
			// 是否未被占用
			if ((block_map[i] & (1 << j)) == 0)
			{
				// 使用空闲的数据块，并转换其标识位
				block_map[i] |= 1 << j;
				blocknr = i * 8 + j;
				return blocknr;
			}
		}
	}
	printf("block_map is empty.\n");
	exit(0);
}

/**
 * @brief 向文件系统文件写入数据
 *
 * @param fp 文件系统文件索引
 * @param offset 偏移量（字节为单位）
 * @param buffer 需要写入的字节数组
 * @param size 字节数组元素的个数
 */
void write_block(FILE *fp, long int offset, char *buffer, int size)
{
	fseek(fp, offset, SEEK_SET);
	fwrite(buffer, 1, size, fp);
}

/**
 * @brief 读取inode表和数据块视图
 * 
 */
void read_first_two_blocks()
{
	fp_xtfs = fopen(fs_name, "r+");
	fread((char *)inode_table, 1, BLOCK_SIZE, fp_xtfs);
	fread(block_map, 1, BLOCK_SIZE, fp_xtfs);
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
	FILE *fp;
	int filesize;
	int i, j;
	size_t size;
	int blocknr;
	char buffer[BLOCK_SIZE];

	fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	memset((char *)index_table, 0, BLOCK_SIZE);
	// 将整个文件读入到文件系统中（一些文件可能超过BLOCK_SIZE即512字节大小）
	// i用于指示是否读入完全
	for (i = 0, j = 0; i < filesize; i += BLOCK_SIZE, j++)
	{
		blocknr = get_block();
		// 读取文件内容，以每个元素1个字节读入到512大小的字节数组buffer中
		// 后续读取会以上次读取停留的指针继续
		size = fread(buffer, 1, BLOCK_SIZE, fp);
		write_block(fp_xtfs, blocknr * BLOCK_SIZE, buffer, size);
		index_table[j] = blocknr;
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
	write_block(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char *)index_table, BLOCK_SIZE);
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
		exit(0);
	}
}

/**
 * @brief 将更新的inode表和数据块视图写回文件系统
 * 
 */
void write_first_two_blocks()
{
	write_block(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
	write_block(fp_xtfs, 512, block_map, BLOCK_SIZE);
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
	int filesize;
	short index_table_blocknr;
	// 文件的数据块索引表
	// 数据块索引表用于记录文件在此文件系统中占用了哪些数据块，存放该文件占用的所有数据块的块号
	// 块号即i * 8 + j
	short index_table[BLOCK_SIZE / 2];
	char *filename;
	char type;

	// 获取待拷贝文件名和文件类型
	filename = argv[1];
	type = atoi(argv[2]);
	fs_name = argv[3];
	// 读取0号和1号inode表和数据块位图数据到进程管理的内存（数组），便于修改
	// 全局变量：inode_table 和 block_map
	read_first_two_blocks();
	// 将文件中内容拷贝到xtfs文件系统中的数据块
	filesize = copy_blocks(filename, index_table);
	// 为文件在xtfs文件系统中申请一个空闲数据块，用于存放其数据块索引表
	index_table_blocknr = write_index_table((char *)index_table);
	// 在inode表中申请一个空闲inode，存放文件的inode信息
	get_empty_inode(filename, filesize, index_table_blocknr, type);
	// 将0号和1号数据块内容写回并关闭文件系统文件
	write_first_two_blocks();

	return(EXIT_SUCCESS);
}