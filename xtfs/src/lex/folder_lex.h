/**
 * @file folder_lex.h
 * @author RoyenHeart
 * @brief 正则目录/文件匹配（声明）
 * @version 1.0.0
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

/**
 * @brief 根据完整的文件路径提取出各个目录和文件元素
 *
 * @param strs 完整的文件路径
 * @param store 提取出的目录、文件元素数组
 * @return int
 * 读取到的目录、文件元素数组有效元素上限（0开始），若解析失败，返回-1
 * @bug 若读取失败，对 store 的内存销毁并没有完善
 */
int get_folders(const char *strs, char ***store);