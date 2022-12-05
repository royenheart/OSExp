/**
 * @file xtfs_check.h
 * @author
 * @brief 文件系统相关检查（声明）
 * @version 0.1
 * @date 2022-11-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

/**
 * @brief 检查进行操作的文件名称
 * 
 * @param name 需要操作的文件名称
 * @deprecated 文件名称的格式已经由 lex 正则表达式保证，此函数在已进行 lex 正则表达式匹配的情况下为冗余函数。
 * @see src/lex/folder_name.l
 */
void check_file_name(const char* name);

/**
 * @brief 检查需要打开的文件系统名称是否符合要求
 * 
 * @param name 需要打开的文件系统名称
 */
void check_fs_name(const char* name);