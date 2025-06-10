/**
 * @file HuffmanZip.cpp
 * @author MDND
 * @brief XTFS 文件系统分区文件压缩（哈夫曼方法）
 * @version 1.0.0
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <bits/stdc++.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>


using namespace std;

extern "C" {
#include "../io.h"
#include "../lex/folder_lex.h"
#include "../xtfs_check.h"
#include "../xtfs_limits.h"
#include "../xtfs_manage.h"
#include "../xtfs_struct.h"
}

#define CHARSET_SIZE 256

struct node {
    int lson, rson;
    int val = -1;
    int num = 0;
    int alpha;
} ls, rs;

bool operator<(node a, node b) { return a.val > b.val; }
bool operator>(node a, node b) { return a.val < b.val; }
string file;
string file_trans;
int filesize;
vector<node> vec;
priority_queue<node> pq;
string s = "";
int appear[CHARSET_SIZE];
string encoder[CHARSET_SIZE];
int col = 0;
string tree_stack;
string alpha_list;
int tree_stack_size;
vector<int> vec_alpha;

char fs_name[MAX_FS_NAME_LENGTH + 1] = {0};
FILE *fp_xtfs = NULL;
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
struct inode inode_table[NR_INODE];

int read_blocks(char *filename) {
    FILE *fp = NULL;
    int need;
    size_t size;
    char buffer[BLOCK_SIZE];

    fp = fopen(filename, "r");
    filesize = read_file_size(fp);
    need = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int i = 0; i < need; i++) {
        size = fread(buffer, 1, BLOCK_SIZE, fp);
        for (int j = 0; j < size; j++) {
            file += buffer[j];
        }
    }
    fclose(fp);
    return filesize;
}

void dfs(int num) {
    if (vec[num].lson) {
        tree_stack += '0';
        tree_stack_size++;
        s += "0";
        dfs(vec[num].lson);
        s.pop_back();
        tree_stack += '1';
        tree_stack_size++;
    }
    if (vec[num].rson) {
        tree_stack += '0';
        tree_stack_size++;
        s += "1";
        dfs(vec[num].rson);
        s.pop_back();
        tree_stack += '1';
        tree_stack_size++;
    }
    if (!vec[num].lson && !vec[num].rson) {
        encoder[vec[num].alpha] = s;
        vec_alpha.push_back(vec[num].alpha);
    }
}

inline void trans() {
    for (int i = 0; i < filesize; i++) {
        int x = (int)file[i];
        for (int j = 0; j < encoder[x].length(); j++) {
            file_trans += (encoder[x][j]);
        }
    }
}

void HuffmanZip() {
    memset(appear, 0, sizeof(appear));
    for (int i = 0; i < filesize; i++) {
        appear[file[i]]++;
    }
    node *newnode = new node();
    vec.push_back(*newnode);
    int num_char = 0;
    for (int i = 0; i < CHARSET_SIZE; i++) {
        if (appear[i]) {
            num_char++;
            newnode->num = ++col;
            newnode->alpha = i;
            newnode->lson = 0;
            newnode->rson = 0;
            newnode->val = appear[i];
            vec.push_back(*newnode);
            pq.push(*newnode);
        }
    }
    while (!pq.empty()) {
        ls = pq.top();
        pq.pop();
        if (pq.empty()) {
            break;
        }
        rs = pq.top();
        pq.pop();
        newnode->num = ++col;
        newnode->alpha = 0;
        newnode->lson = ls.num;
        newnode->rson = rs.num;
        newnode->val = ls.val + rs.val;
        vec.push_back(*newnode);
        pq.push(*newnode);
    }
    tree_stack += '0';
    tree_stack_size++;
    dfs(vec.size() - 1);
    tree_stack += '1';
    tree_stack_size++;
    trans();
    string num1 = "", num2 = "", num3 = "";
    filesize = file_trans.length();
    for (int i = 31; i >= 0; i--) {
        if (num_char & (1 << i)) {
            num1 += '1';
        } else {
            num1 += '0';
        }
        if (tree_stack_size & (1 << i)) {
            num2 += '1';
        } else {
            num2 += '0';
        }
        if (filesize & (1 << i)) {
            num3 += '1';
        } else {
            num3 += '0';
        }
    }
    for (int i = 0; i < vec_alpha.size(); i++) {
        for (int j = 0; j <= 7; j++) {
            if (vec_alpha[i] & (1 << j)) {
                alpha_list += '1';
            } else {
                alpha_list += '0';
            }
        }
    }
    file_trans = num1 + num2 + num3 + tree_stack + alpha_list + file_trans;
    file = "";
    filesize = file_trans.length();
    for (int i = 0; i < filesize;) {
        unsigned char c = 0;
        if (i + 8 < filesize) {
            for (int j = i; j < i + 8; j++) {
                if (file_trans[j] == '1') {
                    c = c + (1 << (j - i));
                }
            }
            i += 8;
            file += ((unsigned char)c);
        } else {
            for (int j = i; j < filesize; j++) {
                if (file_trans[j] == '1') {
                    c = c + (1 << (j - i));
                }
            }
            file += ((unsigned char)c);
            break;
        }
    }
    filesize = file.length();
}

int main(int argc, char *argv[]) {
    char **dirnames = NULL;
    int dir_num;
    int i;
    FILE *fp = NULL;
    // 目录所需数据
    CATALOG dir_index_table[CATALOG_TABLE_SIZE];
    // 文件所需数据
    int inode_blocknr;
    size_t filesize;
    int type;
    INDEX_TABLE_STRUC index_table_blocknr;
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};

    // 初始化 lowbit 表
    for (int i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    check_fs_name(argv[3]);

    dir_num = get_folders(argv[1], &dirnames);

    if (dir_num == ERROR_PARSE) {
        printf("zip failed, check the input file format!\n");
        xtfs_exit(EXIT_FAILURE);
    }

    type = get_file_type(atoi(argv[2]) | ZIP);
    if (is_same_type_class(type, DIR_FILE) == 1) {
        printf("Can't zip all folder right now!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    strncpy(fs_name, argv[3], MAX_FS_NAME_LENGTH);
    fp_xtfs = fopen(fs_name, "r+");

    read_first_two_blocks(fp_xtfs, inode_table, block_map);

    inode_blocknr = get_root_inode(inode_table);
    if (inode_blocknr == NOT_FOUND) {
        printf(
            "Root has been destroyed! This file system may not be in secure "
            "state!\n");
        fclose(fp_xtfs);
        xtfs_exit(EXIT_FAILURE);
    }
    index_table_blocknr = inode_table[inode_blocknr].index_table_blocknr;
    read_dir_index_table(fp_xtfs, dir_index_table,
                         index_table_blocknr * BLOCK_SIZE);

    for (i = 0; i < dir_num; i++) {
        int child_in_father_index;
        child_in_father_index =
            find_dir_index_table(dirnames[i], dir_index_table, DIR_FILE);
        if (child_in_father_index == NOT_FOUND) {
            printf("No such dir!\n");
            fclose(fp_xtfs);
            xtfs_exit(EXIT_FAILURE);
        } else {
            int curr_inode = dir_index_table[child_in_father_index].pos;
            index_table_blocknr = inode_table[curr_inode].index_table_blocknr;
            read_dir_index_table(fp_xtfs, dir_index_table,
                                 index_table_blocknr * BLOCK_SIZE);
            inode_blocknr = curr_inode;
        }
    }

    // 在inode表中申请一个空闲inode，存放文件的inode信息
    inode_blocknr = get_empty_inode(inode_table, dirnames[dir_num], type);
    // 在目录中添加对应的表项，并重新写回文件系统分区
    get_empty_dir_index(dir_index_table, dirnames[dir_num], type,
                        inode_blocknr);
    write_file(fp_xtfs, index_table_blocknr * BLOCK_SIZE,
               (char *)dir_index_table, CATALOG_TABLE_SIZE * sizeof(CATALOG));

    read_blocks(argv[1]);
    HuffmanZip();
    filesize = file.length();
    int pos = 0;
    spec_huffman_zip_params_load(filesize, &pos, file.c_str());
    index_table_blocknr =
        copy_blocks(filesize, type, block_map, lowbit, fp_xtfs);

    inode_table[inode_blocknr].size = filesize;
    inode_table[inode_blocknr].index_table_blocknr = index_table_blocknr;
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);
    return (EXIT_SUCCESS);
}