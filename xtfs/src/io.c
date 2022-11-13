/**
 * @file io.c
 * @author 
 * @brief 基础文件读写模块（实现）
 * @version 0.1
 * @date 2022-11-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "io.h"

inline size_t write_file(FILE *fp, long int offset, char *buffer, size_t size) {
	size_t ret1 = fseek(fp, offset, SEEK_SET);
	size_t ret2 = fwrite(buffer, 1, size, fp);
	ret2 = (ret2 == size)?0:1;
	return ret1 | ret2;
}

inline size_t read_file(FILE *fp, long int offset, char *buffer, size_t size) {
	size_t ret1 = fseek(fp, offset, SEEK_SET);
	size_t ret2 = fread(buffer, 1, size, fp);
	ret2 = (ret2 == size)?0:1;
	return ret1 | ret2;
}