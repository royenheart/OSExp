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

int get_file_type(int i) {
    // 去除特定格式
    int ex_i = i & ~SPEC_TYPES;
    int use_spec = (ex_i == i)?0:1;
    if (i <= NO_FILE || ex_i == 0) {
        printf("Not a file type!\n");
        #ifndef DEBUG
        xtfs_exit(EXIT_FAILURE);
        #else
        return NO_FILE;
        #endif
    }
    if (i > MAX_TYPE_NUM) {
        printf("No such file type!\n");
        #ifndef DEBUG
        xtfs_exit(EXIT_FAILURE);
        #else
        return NO_FILE;
        #endif
    }
    if (ex_i >= UNKNOWN_FILE) {
        printf("Unknown File Type!\n");
        i = i & UNKNOWN_CAL;
    }
    if (use_spec && ex_i == DIR_FILE) {
        printf("Dir file not support special format!\n");
        #ifndef DEBUG
        xtfs_exit(EXIT_FAILURE);
        #else
        return NO_FILE;
        #endif
    }
    return i;
}

inline char is_basic_type(int type) {
    type = type & ~SPEC_TYPES;
    if (type == TEXT_FILE || type == EXE_FILE) {
        return BASIC_TYPE;
    }
    return NO_TYPE;
}

inline char is_dir(int type) {
    type = type & ~SPEC_TYPES;
    return (type == DIR_FILE)?DIR_TYPE:NO_TYPE;
}

inline char is_same_type_class(int type1, int type2) {
    type1 = type1 & ~SPEC_TYPES;
    type2 = type2 & ~SPEC_TYPES;
    int b_type1 = is_basic_type(type1) | is_dir(type1);
    int b_type2 = is_basic_type(type2) | is_dir(type2);
    if (type1 == type2) {
        return 1;
    } else if (b_type1 == b_type2) {
        return 1;
    } else {
        return 0;
    }
}

inline char is_spec_format(int type, int spec) {
    return ((type | spec) != type)?1:0;
}