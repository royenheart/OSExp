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

// 最多支持32种文件类型，文件类型以int数据格式计算

#define NO_FILE 0
#define NOT_FOUND -1

// 特定格式

// 加密后，1000000
#define CIPHER 64
// 压缩后，100000
#define ZIP 32
// 去除特定格式计算用
#define SPEC_TYPES (CIPHER | ZIP)

// 基本格式

// 文本文件，1
#define TEXT_FILE 1
// 可执行文件，10
#define EXE_FILE 2
// 不确定文件，1010，需要比其他基本格式大
#define UNKNOWN_FILE 10
// 在文件类型在32个支持的文件系统中并不存在，统一为不确定的文件
#define UNKNOWN_CAL (SPEC_TYPES | UNKNOWN_FILE)

// 文件类型运算支持

// 最多支持文件类型个数，11111
#define SUPPORT_TYPES 31
// 最大文件类型
#define MAX_TYPE_NUM (SPEC_TYPES | SUPPORT_TYPES)


/**
 * @brief Define the file type according to the params given
 *
 * @param i param indicate the file type
 * @return unsigned char file_type
 */
unsigned char get_file_type(int i);

struct inode {
    int size;
    // 存放文件的数据块索引表所占用的数据块的块号
    short index_table_blocknr;
    // 0保留出来指示当前项并没有写入inode
    unsigned char type;
    char filename[MAX_FILE_NAME_LENGTH];
};