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
// #define INIT_XTFS_MANAGE extern void** pool; pool = (void**)malloc(MAX_POOL_LENGTH * sizeof(void*)); extern specs_copy *specs_params; specs_params = (specs_copy*)malloc(sizeof(specs_params));

// #define max( x , y ) (((x) > (y))?(x):(y))
// #define min( x , y ) (((x) < (y))?(x):(y))

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

typedef struct normal_params normal_params;
struct normal_params {
    FILE* file;
};

/**
 * @brief 普通格式文件buffer写入
 * 
 * @param buffer 待处理的字节数组
 * @return int 处理结果
 * @see struct normal_params
 */
int normal_copy(char *buffer);

/**
 * @brief 装载普通copy需要的参数
 * 
 * @param file 打开的文件
 * @return int 装载成功标志，0为成功
 */
int normal_params_load(FILE* file);

typedef struct cipher_params cipher_params;
struct cipher_params {
    FILE* file;
    unsigned int pwd;
};

/**
 * @brief 进行加密
 * 
 * @param buffer 待处理的字节数组
 * @return int 处理结果
 * @see struct cipher_params
 */
int spec_cipher_copy(char *buffer);

/**
 * @brief 装载加密方法需要的参数
 * 
 * @param pwd 密码
 * @param file 打开的文件
 * @return int 装载成功标志，0为成功
 */
int spec_cipher_params_load(unsigned int pwd, FILE* file);

typedef struct huffman_params huffman_params;
struct huffman_params {
    int file_size;
    int *pos;
    const char *file;
};

/**
 * @brief 进行哈夫曼压缩
 * 
 * @param buffer 字节数组
 * @return int 处理结果
 * @see struct huffman_params
 */
int spec_huffman_zip_copy(char *buffer);

/**
 * @brief 装载huffman压缩方法需要的参数
 * 
 * @param file_size 压缩后的文件大小
 * @param pos huffman目前位置
 * @param file 压缩后的文件内容（字节数组）
 * @return int 装载成功标志，0为成功
 */
int spec_huffman_zip_params_load(int file_size, int *pos, const char *file);

typedef struct specs_copy specs_copy;
struct specs_copy {
    cipher_params cipher_params;
    huffman_params huffman_params;
    normal_params normal_params;
};

/**
 * @brief 在写进文件系统（使用Buffer字节数组）时，根据不同的文件类型对其进行处理
 * 
 * @param buffer 待写入文件系统的Buffer字节数组，默认为BLOCK_SIZE，后面可能需要适应更改
 * @param file_type 文件类型
 * @return int 处理结果
 */
int select_spec_funcs(char *buffer, int file_type);

/**
 * @brief 在装载好文件写入的必要参数后，根据文件大小和文件类型将文件内容和对应的index_table全部先申请后再写入文件系统。
 * 
 * @param file_size 文件大小，用于计算存储空间和需要的数据块索引表的个数
 * @param file_type 文件类型，用于选择将文件内容写入文件系统前需要进行的操作
 * @param block_map 需要进行修改的数据块位图表
 * @param lowbit lowbit表
 * @param fp_xtfs 打开的文件系统
 * @return short 返回第一个数据块索引表的位置 
 */
short copy_blocks(int file_size, int file_type, BLOCK_MAP_STRUC* block_map, BLOCK_MAP_TABLE_STRUC* lowbit, FILE* fp_xtfs);