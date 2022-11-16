/**
 * @file xtfs_struct.c
 * @author
 * @brief xtfs文件系统相关结构（实现）
 * @version 0.1
 * @date 2022-11-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "xtfs_struct.h"
#include "xtfs_manage.h"
#include <stdio.h>
#include <stdlib.h>

unsigned char get_file_type(int i) {
    int ex_i = i & ~SPEC_TYPES;
    if (i <= NO_FILE || ex_i == 0) {
        printf("Not a file type!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    if (i > MAX_TYPE_NUM) {
        printf("No such file type!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    if (ex_i >= UNKNOWN_FILE) {
        printf("Unknown File Type!\n");
        i = i & UNKNOWN_CAL;
    }
    return i;
}
