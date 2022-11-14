/**
 * @file xtfs_struct.h
 * @author 
 * @brief xtfs文件系统相关结构（声明）
 * @version 0.1
 * @date 2022-11-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "xtfs_limits.h"

#define NO_FILE 0
#define TEXT_FILE 1
#define EXE_FILE 2
#define UNKNOWN_FILE 255

/**
 * @brief Define the file type according to the params given
 * 
 * @param i param indicate the file type
 * @return unsigned char file_type
 */
unsigned char get_file_type(int i);

struct inode
{ 
	int size;
	// 存放文件的数据块索引表所占用的数据块的块号
	short index_table_blocknr;
	// 0保留出来指示当前项并没有写入inode
	unsigned char type;
	char filename[MAX_FILE_NAME_LENGTH];
};