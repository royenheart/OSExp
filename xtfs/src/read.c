#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xtfs_limits.h"
#include "xtfs_struct.h"

// 读取的inode表
struct inode inode_table[NR_INODE];
// 文件系统文件名
char *fs_name;

/**
 * @brief 读取inode表
 * 
 */
void read_inode_block()
{
    // 文件系统文件索引
    FILE *fp_xtfs;
	fp_xtfs = fopen(fs_name, "r");
    fseek(fp_xtfs, 0, SEEK_SET);
	fread((char *)inode_table, 1, BLOCK_SIZE, fp_xtfs);
    fclose(fp_xtfs);
}

void read_from_block(long int offset, char* buffer, int size) {
    // 文件系统文件索引
    FILE *fp_xtfs;
    fp_xtfs = fopen(fs_name, "r");
    fseek(fp_xtfs, offset, SEEK_SET);
    fread(buffer, 1, size, fp_xtfs);
    fclose(fp_xtfs);
}

int main(int argc, char* argv[]) {
    char filename[MAX_FILE_NAME_LENGTH] = {0};
    short index_table[BLOCK_SIZE / 2];
    int i;

    strcpy(filename, argv[1]);
    fs_name = argv[2];
    read_inode_block();
    for (i = 0; i < NR_INODE; i++) {
        // 未加入目录，目前只以文件名判断
        if (strcmp(inode_table[i].filename, filename) == 0) {
            long int blocknr = inode_table[i].index_table_blocknr;
            read_from_block(blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            break;
        }
    }

    if (i == NR_INODE) {
        printf("No such file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // 根据数据块索引表读入文件数据
    for (i = 0; i < BLOCK_SIZE / 2; i++) {
        char data[BLOCK_SIZE];
        int block_pos = index_table[i];
        if (block_pos == 0) {
            break;
        }
        read_from_block(block_pos * BLOCK_SIZE, data, BLOCK_SIZE);
        printf("%s", data);
    }
    
    return(EXIT_SUCCESS);
}