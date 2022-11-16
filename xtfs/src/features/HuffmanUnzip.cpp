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

#include<bits/stdc++.h>

using namespace std;

extern "C" {
#include "../xtfs_limits.h"
#include "../xtfs_struct.h"
#include "../xtfs_manage.h"
#include "../xtfs_check.h"
#include "../io.h"
}

int num_char, t_stack_size, filesize;
string tree_stack;
string file;
int nownode, nodecnt = 0;
string io_file;
char buffer[1000005];
int ls[1000005], rs[1000005], fa[1000005], val[1000005];
int alpha[1000005];
string alpha_list;
int oz_num;
char* fs_name = NULL;
FILE* fp_xtfs = NULL;
struct inode inode_table[NR_INODE];

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

string s = "";

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
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    int i;

    check_file_name(argv[1]);
    check_fs_name(argv[2]);

    strcpy(filename, argv[1]);
    fs_name = argv[2];
    fp_xtfs = fopen(fs_name, "r+");

    read_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);
    // int file_siz = 0;
    for (i = 0; i < NR_INODE; i++) {
        if (strcmp(inode_table[i].filename, filename) == 0) {
            long int blocknr = inode_table[i].index_table_blocknr;
            filesize = inode_table[i].size;
            read_file(fp_xtfs, blocknr * BLOCK_SIZE, (char*)index_table, BLOCK_SIZE);
            break;
        }
    }

    for (i = 0; i < BLOCK_SIZE / 2; i++) {
        char data[BLOCK_SIZE] = {0};
        int block_pos = index_table[i];
        if (block_pos == 0) {
            break;
        }
        read_file(fp_xtfs, block_pos * BLOCK_SIZE, data, BLOCK_SIZE);
        for (int j = 0; j < min(512, filesize); j++) {
            for (int k = 0; k <= 7; k++) {
                if (data[j] & (1 << k)) {
                    io_file += '1';
                } else {
                    io_file += '0';
                }
            }
        }
        filesize -= 512;
    }

    for (int i = 0; i <= 15; i++) {
        if (io_file[15 - i] == '1') {
            num_char += (1 << i);
        }
        if (io_file[31 - i] == '1') {
            t_stack_size += (1 << i);
        }
        if (io_file[47 - i] == '1') {
            oz_num += (1 << i);
        }
    }

    io_file.erase(0, 48);
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
    file = io_file.substr(0,oz_num);
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
