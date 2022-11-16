/**
 * @file HuffmanZip.cpp
 * @author MDND
 * @brief 哈夫曼方法-压缩
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <bits/stdc++.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>

using namespace std;

extern "C" {
#include "../xtfs_limits.h"
#include "../xtfs_struct.h"
#include "../xtfs_manage.h"
#include "../xtfs_check.h"
#include "../io.h"
}

#define CHARSET_SIZE 256
#define NAME_LEN 9

struct node {
    int lson, rson;
    int val = -1;
    int num = 0;
    int alpha;
} ls, rs;

bool operator <(node a, node b) {return a.val > b.val;}
bool operator >(node a, node b) {return a.val < b.val;}
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

char* fs_name = NULL;
FILE* fp_xtfs = NULL;
BLOCK_MAP_TABLE_STRUC lowbit[BLOCK_MAP_TABLE_SIZE];
BLOCK_MAP_STRUC block_map[BLOCK_SIZE];
struct inode inode_table[NR_INODE];

int read_blocks(char* filename) {
    FILE* fp = NULL;
    int need;
    size_t size;
    char buffer[BLOCK_SIZE];

    fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
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
        tree_stack += '0'; tree_stack_size++;
        s += "0";
        dfs(vec[num].lson);
        s.pop_back();
        tree_stack += '1'; tree_stack_size++;
    }
    if (vec[num].rson) {
        tree_stack += '0'; tree_stack_size++;
        s += "1";
        dfs(vec[num].rson);
        s.pop_back();
        tree_stack += '1'; tree_stack_size++;
    }
    if (!vec[num].lson && !vec[num].rson) {
        encoder[vec[num].alpha] = s;
        vec_alpha.push_back(vec[num].alpha);
    }
}

inline void trans() {
    for (int i = 0; i < filesize; i++) {
        int x = (int)file[i];
        for (int j = 0; j < encoder[x].length(); j++) { //fout<<j<<endl;
            file_trans += (encoder[x][j]);
        }
    }
}

void HuffmanZip() {
    memset(appear, 0, sizeof(appear));
    //filesize--;
    for (int i = 0; i < filesize; i++) {
        appear[file[i]]++;
    }
    node* newnode = new node();
    vec.push_back(*newnode);
    int num_char = 0;
    for (int i = 0; i < CHARSET_SIZE; i++) {
        if (appear[i]) {
            num_char++;
            newnode -> num = ++col;
            newnode -> alpha = i;
            newnode -> lson = 0;
            newnode -> rson = 0;
            newnode -> val = appear[i];
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
        newnode -> num = ++col;
        newnode -> alpha = 0;
        newnode -> lson = ls.num;
        newnode -> rson = rs.num;
        newnode -> val = ls.val + rs.val;
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
    for (int i = 15; i >= 0; i--) {
        if (num_char & (1 << i))
            num1 += '1';
        else
            num1 += '0';

        if (tree_stack_size & (1 << i))
            num2 += '1';
        else
            num2 += '0';

        if (filesize & (1 << i))
            num3 += '1';
        else
            num3 += '0';
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

int copy_blocks(short* index_table) {
    int filesize;
    int i, need;
    int blocknr;
    size_t size;
    char buffer[BLOCK_SIZE];

    filesize = file.length();

    memset((char*)index_table, 0, BLOCK_SIZE);
    need = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int pos = 0;
    for (i = 0; i < need; i++) {
        blocknr = get_block(block_map, lowbit);
        size = min(512, filesize - pos);
        for (int j = pos; j < pos + size; j++) {
            buffer[j - pos] = file[j];
        }
        pos += size;
        write_file(fp_xtfs, blocknr * BLOCK_SIZE, buffer, BLOCK_SIZE);
        index_table[i] = blocknr;
    }
    return filesize;
}

int main(int argc, char* argv[]) {
    int inode_nr;

    for (int i = 0; i < 8; i++) {
        lowbit[1 << i] = i;
    }

    check_file_name(argv[1]);
    check_fs_name(argv[3]);

    FILE* fp = NULL;
    int filesize;
    int type;
    char* filename = NULL;
    INDEX_TABLE_STRUC index_table[INDEX_TABLE_SIZE] = {0};
    int i, blocknr, index_table_blocknr;

    filename = argv[1];
    type = get_file_type(atoi(argv[2]) | ZIP);
    fs_name = argv[3];
    fp_xtfs = fopen(fs_name, "r+");

    read_first_two_blocks(fp_xtfs, inode_table, block_map);
    filesize = read_blocks(filename);
    HuffmanZip();
    filesize = copy_blocks(index_table);
    index_table_blocknr = write_index_table(fp_xtfs, block_map, lowbit, index_table);
    inode_nr = get_empty_inode(inode_table, filename, type);
    inode_table[inode_nr].size = filesize;
    inode_table[inode_nr].index_table_blocknr = index_table_blocknr;
    write_first_two_blocks(fp_xtfs, inode_table, block_map);

    fclose(fp_xtfs);
    return (EXIT_SUCCESS);
}