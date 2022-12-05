/**
 * @file xtfs_limits.h
 * @author RoyenHeart
 * @brief XTFS 文件系统限制
 * @version 1.0.0
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
// 数据块位图 lowbit 表大小（用于判断j）
#define BLOCK_MAP_TABLE_SIZE 1024
// 数据块位图 lowbit 表类型
#define BLOCK_MAP_TABLE_STRUC unsigned char

// 数据块索引表类型
#define INDEX_TABLE_STRUC short
// 数据块索引表大小
#define INDEX_TABLE_SIZE (BLOCK_SIZE / sizeof(INDEX_TABLE_STRUC))
// 数据块索引表有效指向数据区大小
#define INDEX_TABLE_DATA_SIZE (INDEX_TABLE_SIZE - 1)

// 目录的数据块索引表类型
typedef struct catalog CATALOG;
// 目录的数据块索引表大小
#define CATALOG_TABLE_SIZE (BLOCK_SIZE/sizeof(CATALOG))
// 目录的有效数据块索引表大小，最后一块空间作跳转用
#define CATALOG_TABLE_DATA_SIZE (CATALOG_TABLE_SIZE - 1)

// 最大文件名（对于目录也一样）长度
#define MAX_FILE_NAME_LENGTH 20
// 最大文件系统文件名长度
#define MAX_FS_NAME_LENGTH 20
// 最大目录深度
#define MAX_ITER_FOLDER 100