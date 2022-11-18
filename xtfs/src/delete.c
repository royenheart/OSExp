/**
 * @file delete.c
 * @author bow
 * @brief xtfs文件系统文件删除
 * @version 0.1
 * @date 2022-11-14
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
char* fs_name;
FILE* fp_xtfs = NULL;

int main(int argc, char* argv[]) {
    INDEX_TABLE_STRUC index_table_blocknr;
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    INDEX_TABLE_STRUC exist;
    char* filename;
    int filesize;
    int i;

    check_file_name(argv[1]);
    check_fs_name(argv[2]);

    // 读取0号和1号inode表和数据块位图数据到进程管理的内存（数组），便于修改
    filename = argv[1];
    fs_name = argv[2];

    fp_xtfs = fopen(fs_name, "r+");

    // 加载前两个数据块到 inode_table 和 block_map数组中
    read_first_two_blocks(fp_xtfs, inode_table, block_map);

    int inode_index_table = find_inode_index_table(filename, inode_table);
    if (inode_index_table == NOT_FOUND) {
        // 返回-1表示文件不存在
        printf("The file %s does not exist!\n", filename); 
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }
    // 得到第一个数据块索引表
    index_table_blocknr = inode_table[inode_index_table].index_table_blocknr;
    read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
    // 读取文件大小
    filesize = inode_table[inode_index_table].size;
    // 数据块内容已经存储的数据块
    exist = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // 将第一个数据块索引表在数据块位图中置0
    set_block_map(0, index_table_blocknr, block_map);

    for (i = 0; i < exist; i++) {
        // 检查 index_table ，得到应该读取的正确位置，若当前 index_table 已经读满，则进入下一个。
        INDEX_TABLE_STRUC index = i;
        if ((index + 1) % (INDEX_TABLE_DATA_SIZE + 1) == 0) {
            index_table_blocknr = index_table[INDEX_TABLE_DATA_SIZE];
            memset(index_table, 0, INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
            read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            set_block_map(0, index_table_blocknr, block_map);
            index = 0;
        } else {
            index = index % INDEX_TABLE_DATA_SIZE;
        }
        // 将所有占用的数据块在block_map中置0
        set_block_map(0, index_table[index], block_map);
    }

    // 将对应 inode 表类型置0，即表示为空文件
    inode_table[inode_index_table].type = NO_FILE;
    // 将inode_table和block_map写回0/1号数据块
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);
    
    return EXIT_SUCCESS;
}
