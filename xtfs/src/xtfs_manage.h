/**
 * @file xtfs_manage.h
 * @author 
 * @brief 提供统一的内存管理、退出机制、错误信息管理机制
 * @version 0.1
 * @date 2022-10-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#define MAX_POOL_LENGTH 100
#define INIT_XTFS_MANAGE extern void** pool; pool = (void**)malloc(MAX_POOL_LENGTH * sizeof(void*));

void xtfs_exit(int ret);
void *xtfs_malloc(size_t size);