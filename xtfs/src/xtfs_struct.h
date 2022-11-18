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

// 最多支持31种文件类型，文件类型以int数据格式计算，0保留作无文件（NO_FILE）

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
 * @brief 根据输入的类型判断文件类型，并根据错误、非法输入进行相应的处理
 *
 * @param i 需要判定的文件类型
 * @return unsigned char 判定合法的文件类型
 */
unsigned char get_file_type(int i);

/**
 * @brief 根据输入的文件类型判断是否使用了某种特定格式技术
 * 
 * @param type 需要判断的文件类型
 * @param spec 需要判断的特定格式
 * @return char 0表示该文件类型使用了指定的特定格式；其他数表示没有使用
 */
char is_spec_format(int type, int spec);

struct inode {
    int size;
    // 存放文件的数据块索引表所占用的数据块的块号
    short index_table_blocknr;
    // 0保留出来指示当前项并没有写入inode
    unsigned char type;
    char filename[MAX_FILE_NAME_LENGTH];
};