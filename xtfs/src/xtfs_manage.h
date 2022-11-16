/**
 * @file xtfs_manage.h
 * @author
 * @brief 提供统一的内存管理、退出机制、错误信息管理机制，常用函数封装（声明）
 * @version 0.1
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <stddef.h>
#include <stdio.h>
#include "xtfs_struct.h"

#define MAX_POOL_LENGTH 100
#define INIT_XTFS_MANAGE extern void** pool; pool = (void**)malloc(MAX_POOL_LENGTH * sizeof(void*));


/**
 * @brief 退出进程
 *
 * @param ret 退出返回值
 */
void xtfs_exit(int ret);

/**
 * @brief 释放内存池空间
 *
 * @return void*
 */
void* xtfs_free_pool(void);

/**
 * @brief 在内存池中分配内存
 *
 * @param size 空间大小（字节计）
 * @return void*
 */
void* xtfs_malloc(size_t size);

/**
 * @brief 获取空余数据块
 *
 * @param block_map 读取的数据块位图数组
 * @param lowbit lowbit表数组
 * @return short blocknr 返回在数据块位图的第几个字节的第几位（表示具体哪一块，512 * 8 = 4096）
 */
short get_block(BLOCK_MAP_STRUC* block_map, BLOCK_MAP_TABLE_STRUC* lowbit);

/**
 * @brief 根据需要的块数获取所有可行的数据块编号
 *
 * @param need 需要的数据块数
 * @param block_map 读取的数据块位图数组
 * @param lowbit lowbit表数组
 * @return short* 数据块编号数组
 */
short* get_all_block(int need, BLOCK_MAP_STRUC* block_map, BLOCK_MAP_TABLE_STRUC* lowbit);

/**
 * @brief 将更新的inode表和数据块视图写回文件系统
 *
 * @param fp_xtfs 文件系统
 * @param inode_table inode表
 * @param block_map 数据块位图表
 */
void write_first_two_blocks(FILE* fp_xtfs, struct inode* inode_table, BLOCK_MAP_STRUC* block_map);

/**
 * @brief 读取inode表和数据块视图
 *
 * @param fp_xtfs 文件系统
 * @param inode_table inode表
 * @param block_map 数据块位图表
 */
void read_first_two_blocks(FILE* fp_xtfs, struct inode* inode_table, BLOCK_MAP_STRUC* block_map);

/**
 * @brief 写入文件对应的数据块索引表
 *
 * @param fp_xtfs 文件系统
 * @param block_map 数据块位图表
 * @param lowbit lowbit表
 * @param index_table 数据块索引表
 * @return short index_table_blocknr 存储数据块索引表的块号
 */
short write_index_table(FILE* fp_xtfs, BLOCK_MAP_STRUC* block_map, BLOCK_MAP_TABLE_STRUC* lowbit, INDEX_TABLE_STRUC* index_table);

/**
 * @brief 根据块号设置数据块位图对应flag
 *
 * @param flag 对应块号是否占用
 * @param blocknr 块号
 * @param block_map 数据块位图
 */
void set_block_map(int flag, short blocknr, BLOCK_MAP_STRUC* block_map);

/**
 * @brief 根据inode表和文件名查找存在的文件
 *
 * @param filename 待查找的文件名
 * @param inode_table inode表
 * @return int 文件inode在inode表中的位置，否则返回NOT FOUND
 */
int find_inode_index_table(char* filename, struct inode* inode_table);

/**
 * @brief 将文件inode数据写入inode表
 *
 * @param inode_table inode表
 * @param filename 文件名
 * @param type 文件类型
 * @return int 文件inode结构存储位置
 */
int get_empty_inode(struct inode* inode_table, char* filename, char type);