/**
 * @file xtfs_check.c
 * @author
 * @brief 文件系统相关检查（实现）
 * @version 0.1
 * @date 2022-11-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "string.h"
#include "xtfs_limits.h"
#include "xtfs_manage.h"
#include <stdio.h>

void check_file_name(const char* name) {
    if (strlen(name) > MAX_FILE_NAME_LENGTH) {
        printf("File name too long!: %s\n", name);
        xtfs_exit(EXIT_FAILURE);
    }
}

void check_fs_name(const char* name) {
    if (strlen(name) > MAX_FS_NAME_LENGTH) {
        printf("XTFS name too long!: %s\n", name);
        xtfs_exit(EXIT_FAILURE);
    }
}
