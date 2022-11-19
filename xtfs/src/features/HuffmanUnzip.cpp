/**
 * @file HuffmanUnzip.cpp
 * @author MDND
 * @brief 哈夫曼方法-解压
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <bits/stdc++.h>

using namespace std;

extern "C" {
#include "../xtfs_limits.h"
#include "../xtfs_struct.h"
#include "../xtfs_manage.h"
#include "../xtfs_check.h"
#include "../io.h"
}

char* fs_name = NULL;
FILE* fp_xtfs = NULL;
struct inode inode_table[NR_INODE];

// Huffman Unzip Need
int num_char, t_stack_size, filesize;
string tree_stack;
string file;
int nownode, nodecnt = 0;
string io_file;
char buffer[10000005];
int ls[10000005], rs[10000005], fa[10000005], val[10000005];
int alpha[10000005];
string alpha_list;
int oz_num;
string s = "";
// Huffman Unzip Need

inline void build() {
    nownode = ++nodecnt;
    for (int i = 2; i <= t_stack_size - 1; i++) {
        if (tree_stack[i] == '0') {
            if (ls[nownode]) {
                rs[nownode] = ++nodecnt;
                fa[rs[nownode]] = nownode;
                nownode = rs[nownode];
            } else {
                ls[nownode] = ++nodecnt;
                fa[ls[nownode]] = nownode;
                nownode = ls[nownode];
            }
        } else {
            nownode = fa[nownode];
        }
    }
}

void write() {
    nownode = 1;
    for (int i = 0; i < filesize; i++) {
        if (file[i] == '0') {
            if (ls[nownode]) {
                nownode = ls[nownode];
            } else {
                printf("%c", (char)(val[nownode]));
                nownode = 1;
                i--;
            }
        } else {
            if (rs[nownode]) {
                nownode = rs[nownode];
            } else {
                printf("%c", (char)(val[nownode]));
                nownode = 1;
                i--;
            }
        }
    }
    if (val[nownode]) {
        printf("%c", (char)(val[nownode]));
    }
}

void dfs(int x) {
    if (ls[x]) {
        s += "0";
        dfs(ls[x]);
        s.pop_back();
    }
    if (rs[x]) {
        s += "1";
        dfs(rs[x]);
        s.pop_back();
    }
}

int main(int argc, char* argv[]) {
    char filename[MAX_FILE_NAME_LENGTH] = {0};
    INDEX_TABLE_STRUC index_table_blocknr;
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    INDEX_TABLE_STRUC exist;
    int i, j, k;

    check_file_name(argv[1]);
    check_fs_name(argv[2]);

    strncpy(filename, argv[1], MAX_FILE_NAME_LENGTH);
    fs_name = argv[2];
    fp_xtfs = fopen(fs_name, "r+");

    read_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);

    // for (i = 0; i < NR_INODE; i++) {
    //     if (strcmp(inode_table[i].filename, filename) == 0 && inode_table[i].type != NO_FILE) {
    //         long int blocknr = inode_table[i].index_table_blocknr;
    //         filesize = inode_table[i].size;
    //         read_file(fp_xtfs, blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
    //         break;
    //     }
    // }

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

    // for (i = 0; i < BLOCK_SIZE / 2; i++) {
    for (i = 0; i < exist; i++) {
        char data[BLOCK_SIZE] = {0};
        // 检查 index_table ，得到应该读取的正确位置，否则转换到下一个
        INDEX_TABLE_STRUC index = i;
        if ((index + 1) % (INDEX_TABLE_DATA_SIZE + 1) == 0) {
            index_table_blocknr = index_table[INDEX_TABLE_DATA_SIZE];
            memset(index_table, 0, INDEX_TABLE_SIZE * sizeof(INDEX_TABLE_STRUC));
            read_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            index = 0;
        } else {
            index = index % INDEX_TABLE_DATA_SIZE;
        }
        INDEX_TABLE_STRUC block_pos = index_table[index];
        if (block_pos == 0) {
            break;
        }
        read_file(fp_xtfs, block_pos * BLOCK_SIZE, data, BLOCK_SIZE);
        for (j = 0; j < min(BLOCK_SIZE, filesize); j++) {
            for (k = 0; k <= 7; k++) {
                // 对每个字节进行操作
                if (data[j] & (1 << k)) {
                    io_file += '1';
                } else {
                    io_file += '0';
                }
            }
        }
        filesize -= BLOCK_SIZE;
    }

    // 存文本长度，存了一个int
    for (int i = 0; i <= 31; i++) {
        if (io_file[31 - i] == '1') {
            num_char += (1 << i);
        }
        if (io_file[63 - i] == '1') {
            t_stack_size += (1 << i);
        }
        if (io_file[95 - i] == '1') {
            oz_num += (1 << i);
        }
    }

    io_file.erase(0, 96);
    tree_stack = " " + io_file.substr(0, t_stack_size);
    io_file.erase(0, t_stack_size);
    alpha_list = " " + io_file.substr(0, num_char * 8);
    io_file.erase(0, num_char * 8);

    for (int i = 1; i <= num_char; i++) {
        for (int j = 0; j <= 7; j++) {
            if (alpha_list[(i - 1) * 8 + j + 1] == '1') {
                alpha[i] += (1 << j);
            }
        }
    }

    // 获取解压缩后的文件内容
    file = io_file.substr(0, oz_num);
    filesize = file.length();
    build();

    for (int i = 1; i <= nodecnt; i++) {
        if (!ls[i] && !rs[i]) {
            val[i] = alpha[++alpha[0]];
        }
    }
    
    dfs(1);
    write();

    return 0;
}
