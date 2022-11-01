#pragma once

#include <stdio.h>

/**
 * @brief 文件写入数据
 *
 * @param fp 文件索引
 * @param offset 写入偏移量
 * @param buffer 需要写入的字节数组
 * @param size 字节数组元素的个数
 */
void write_file(FILE* fp, long int offset, char* buffer, size_t size);

/**
 * @brief 读取文件数据
 *
 * @param fp 文件索引
 * @param offset 读出偏移量
 * @param buffer 读出数据存储字节数组
 * @param size 读出字节个数
 */
void read_file(FILE* fp, long int offset, char* buffer, size_t size);