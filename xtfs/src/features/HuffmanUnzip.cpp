/**
 * @file HuffmanUnzip.cpp
 * @author MDND
 * @brief XTFS 文件系统分区文件解压（哈夫曼方法）
 * @version 1.0.0
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
#include "../lex/folder_lex.h"
#include "../io.h"
}

char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
FILE* fp_xtfs = NULL;
struct inode inode_table[NR_INODE];

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
    char** dirnames = NULL;
    int dir_num;
    int i, j, k;
    // 目录所需数据
    CATALOG dir_index_table[CATALOG_TABLE_SIZE];
    // 文件所需数据
    int inode_blocknr;
    int type;
    INDEX_TABLE_STRUC index_table_blocknr;
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    INDEX_TABLE_STRUC exist;

    check_fs_name(argv[3]);

    dir_num = get_folders(argv[1], &dirnames);

    if (dir_num == ERROR_PARSE) {
        printf("unzip failed, check the input file format!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    type = get_file_type(atoi(argv[2]));
    if (is_same_type_class(type, DIR_FILE) == 1) {
        printf("Can't unzip a dir!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    strncpy(fs_name, argv[3], MAX_FS_NAME_LENGTH);
    fp_xtfs = fopen(fs_name, "r+");

    read_file(fp_xtfs, 0, (char*)inode_table, BLOCK_SIZE);

    inode_blocknr = get_root_inode(inode_table);
    if (inode_blocknr == NOT_FOUND) {
        printf("Root has been destroyed! This file system may not be in secure state!\n");
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }
    index_table_blocknr = inode_table[inode_blocknr].index_table_blocknr;
    read_dir_index_table(fp_xtfs, dir_index_table, index_table_blocknr * BLOCK_SIZE);

    for (i = 0; i <= dir_num; i++) {
        int child_in_father_index;
        child_in_father_index = find_dir_index_table(dirnames[i], dir_index_table, (i < dir_num) ? DIR_FILE : type | ZIP);
        if (child_in_father_index == NOT_FOUND) {
            printf("No such file %s with type %d!\n", argv[1], type);
            fclose(fp_xtfs);
            xtfs_exit(EXIT_FAILURE);
        } else {
            int curr_inode = dir_index_table[child_in_father_index].pos;
            index_table_blocknr = inode_table[curr_inode].index_table_blocknr;
            if (i < dir_num) {
                read_dir_index_table(fp_xtfs, dir_index_table, index_table_blocknr * BLOCK_SIZE);
            } else {
                read_index_table(fp_xtfs, index_table, index_table_blocknr * BLOCK_SIZE);
            }
            inode_blocknr = curr_inode;
        }
    }

    // 读取文件大小
    filesize = inode_table[inode_blocknr].size;
    // 数据块内容已经存储的数据块
    exist = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // for (i = 0; i < BLOCK_SIZE / 2; i++) {
    for (i = 0; i < exist; i++) {
        char data[BLOCK_SIZE] = {0};
        // 检查 index_table ，得到应该读取的正确位置，否则转换到下一个
        INDEX_TABLE_STRUC index = i;
        if (index && index % INDEX_TABLE_DATA_SIZE == 0) {
            index_table_blocknr = index_table[INDEX_TABLE_DATA_SIZE];
            read_index_table(fp_xtfs, index_table, index_table_blocknr * BLOCK_SIZE);
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
