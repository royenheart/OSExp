/**
 * @file xtfs_limits.h
 * @author 
 * @brief xtfs文件系统限制
 * @version 0.1
 * @date 2022-11-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdlib.h>

// 数据块大小
#define BLOCK_SIZE 512
// inode数据结构大小
#define INODE_SIZE sizeof(struct inode)
// inode表大小
#define NR_INODE (BLOCK_SIZE / INODE_SIZE)
// 数据块位图类型
#define BLOCK_MAP_STRUC unsigned char
// 数据块位图lowbit表大小（用于判断j）
#define BLOCK_MAP_TABLE_SIZE 1024
// 最大文件名长度
#define MAX_FILE_NAME_LENGTH 20
// 最大文件系统文件名长度
#define MAX_FS_NAME_LENGTH 20