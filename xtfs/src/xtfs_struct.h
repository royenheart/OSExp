#pragma once

#include "xtfs_limits.h"

struct inode {
    int size;
    // 存放文件的数据块索引表所占用的数据块的块号
    short index_table_blocknr;
    // 0保留出来指示当前项并没有写入inode
    char type;
    char filename[MAX_FILE_NAME_LENGTH];
};