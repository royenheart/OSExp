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
#include <stdio.h>

unsigned char get_file_type(int i) {
    switch (i) {
		case NO_FILE: return NO_FILE;  
		case TEXT_FILE: return TEXT_FILE; 
		case EXE_FILE: return EXE_FILE; 
		default: printf("Unknown File Type: %d!", i); return UNKNOWN_FILE; 
	}
}