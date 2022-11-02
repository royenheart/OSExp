#pragma once

#include <stdio.h>

/**
 * @brief 文件写入数据
 *
 * @param fp 文件索引
 * @param offset 写入偏移量
 * @param buffer 需要写入的字节数组
 * @param size 字节数组元素的个数
 * @return size_t 0:写操作正常;非0:写操作异常
 */
size_t write_file(FILE *fp, long int offset, char *buffer, size_t size);

/**
 * @brief 读取文件数据
 *
 * @param fp 文件索引
 * @param offset 读出偏移量
 * @param buffer 读出数据存储字节数组
 * @param size 读出字节个数
 * @return size_t 0:读操作正常;非0:读操作异常
 */
size_t read_file(FILE *fp, long int offset, char *buffer, size_t size);