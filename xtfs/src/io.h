/**
 * @file io.h
 * @author
 * @brief 基础文件读写模块（声明）
 * @version 0.1
 * @date 2022-11-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <stdio.h>

/**
 * @brief 将数据写入文件
 *
 * @param fp 已打开的文件索引
 * @param offset 写入偏移量（字节计算）
 * @param buffer 需要写入的字节数组
 * @param size 字节数组元素的个数
 * @return size_t 0:写操作正常;非0:写操作异常
 * @attention 文件写的原始使用，不检查缓冲区大小等
 */
size_t write_file(FILE* fp, long long offset, char* buffer, size_t size);

/**
 * @brief 先对文件对应块 size 大小的部分全部覆盖，再将数据写入文件，
 * 
 * @param fp 已打开的文件索引
 * @param offset 写入偏移量（字节计算）
 * @param buffer 需要写入的字节数组
 * @param size 字节数组元素的个数
 * @return size_t 0:写操作正常;非0:写操作异常
 * @attention 文件写的原始使用，不检查缓冲区大小等
 */
size_t write_file_with_blank(FILE* fp, long long offset, char* buffer, size_t size);

/**
 * @brief 读取文件的数据
 *
 * @param fp 已打开的文件索引
 * @param offset 读出偏移量（字节计算）
 * @param buffer 读出数据存储字节数组
 * @param size 读出字节个数
 * @return size_t 0:读操作正常;非0:读操作异常
 * @attention 文件读的原始使用，不检查缓冲区大小等
 */
size_t read_file(FILE* fp, long long offset, char* buffer, size_t size);

/**
 * @brief 读取文件大小
 * 
 * @param fp 打开的文件索引
 * @return size_t 文件大小
 */
size_t read_file_size(FILE *fp);