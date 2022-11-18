/**
 * @file copy.c
 * @author RoyenHeart, Ruoxuan Wang, MDND, bow
 * @brief 基础文件系统文件拷贝
 * @version 0.1
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xtfs_limits.h"
#include "xtfs_struct.h"
#include "xtfs_manage.h"
#include "xtfs_check.h"
#include "io.h"

// 读取的inode表
struct inode inode_table[NR_INODE];
// 读取的数据块位图
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
// 数据块位图lowbit表
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
// 文件系统文件名
char* fs_name = NULL;
// 文件系统文件索引
FILE* fp_xtfs = NULL;

// /**
//  * @brief 打开文件并将内容拷贝到数据块中，调用get_block和write_block
//  *
//  * @param filename 文件名
//  * @param index_table 文件数据块索引表
//  * @return int filesize 拷贝的文件大小
//  */
// int copy_blocks(char* filename, short* index_table) {
//     FILE* fp = NULL;
//     int filesize;
//     int i, need;
//     int blocknr;
//     char buffer[BLOCK_SIZE];

//     fp = fopen(filename, "r");
//     fseek(fp, 0, SEEK_END);
//     filesize = ftell(fp);
//     fseek(fp, 0, SEEK_SET);
//     memset((char*)index_table, 0, BLOCK_SIZE);
//     // 将整个文件读入到文件系统中，并更新其数据块索引；
//     // 先获取所有可行块，避免空间不够导致的额外开销
//     need = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;
//     short* blocknr_s = get_all_block(need, block_map, lowbit);
//     for (i = 0; i < need; i++) {
//         blocknr = blocknr_s[i];
//         // 读取文件内容，以每个元素1个字节读入到512大小的字节数组buffer中
//         // 后续读取会以上次读取停留的指针继续
//         memset(buffer, 0, BLOCK_SIZE);
//         fread(buffer, 1, BLOCK_SIZE, fp);
//         write_file(fp_xtfs, blocknr * BLOCK_SIZE, buffer, BLOCK_SIZE);
//         index_table[i] = blocknr;
//     }
//     fclose(fp);
//     return filesize;
// }

int main(int argc, char* argv[]) {
    // INIT_XTFS_MANAGE
    size_t filesize;
    short index_table_blocknr;
    // 文件的数据块索引表
    // 数据块索引表用于记录文件在此文件系统中占用了哪些数据块，存放该文件占用的所有数据块的块号
    // 块号即i * 8 + j
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    char* filename;
    unsigned char type;
    int i;

    // 初始化lowbit表
    for (i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    check_file_name(argv[1]);
    check_fs_name(argv[3]);

    // 获取待拷贝文件名和文件类型
    filename = argv[1];
    // 去除特定格式
    type = get_file_type(atoi(argv[2]) & ~SPEC_TYPES);
    fs_name = argv[3];

    fp_xtfs = fopen(fs_name, "r+");

    // 读取0号和1号inode表和数据块位图数据到进程管理的内存（数组），便于修改
    // 全局变量：inode_table 和 block_map
    read_first_two_blocks(fp_xtfs, inode_table, block_map);
    // 在inode表中申请一个空闲inode，存放文件的inode信息
    // 若以存在相同文件，调用rewrite程序
    i = get_empty_inode(inode_table, filename, type);
    // 将文件中内容拷贝到xtfs文件系统中
    FILE *fp = fopen(filename, "r");
    filesize = read_file_size(fp);
    normal_params_load(fp);
    index_table_blocknr = copy_blocks(filesize, type, block_map, lowbit, fp_xtfs);
    fclose(fp);
    // filesize = copy_blocks(filename, index_table);
    // 将数据块索引表拷贝到xtfs文件系统中
    // index_table_blocknr = write_index_table(fp_xtfs, block_map, lowbit, index_table);
    // 文件复制准备完全后将数据同步至inode表
    inode_table[i].size = filesize;
    inode_table[i].index_table_blocknr = index_table_blocknr;
    // 将0号和1号数据块内容写回并关闭文件系统文件
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);

    return (EXIT_SUCCESS);
}