/**
 * @file io.c
 * @author RoyenHeart
 * @brief 基础文件读写模块（实现）
 * @version 1.0.0
 * @date 2022-11-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "io.h"

#include "xtfs_limits.h"

size_t write_file(FILE *fp, long long offset, char *buffer, size_t size) {
    size_t ret1 = fseek(fp, offset, SEEK_SET);
    size_t ret2 = fwrite(buffer, 1, size, fp);
    ret2 = (ret2 == size) ? 0 : 1;
    return ret1 | ret2;
}

size_t write_file_with_blank(FILE *fp, long long offset, char *buffer,
                             size_t size) {
    char blank[BLOCK_SIZE] = {0};
    size_t ret1 = write_file(fp, offset, blank, size);
    size_t ret2 = write_file(fp, offset, buffer, size);
    return ret1 | ret2;
}

size_t read_file(FILE *fp, long long offset, char *buffer, size_t size) {
    size_t ret1 = fseek(fp, offset, SEEK_SET);
    size_t ret2 = fread(buffer, 1, size, fp);
    ret2 = (ret2 == size) ? 0 : 1;
    return ret1 | ret2;
}

size_t read_file_size(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return filesize;
}