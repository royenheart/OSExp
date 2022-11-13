/**
 * @file xtfs_manage.c
 * @author 
 * @brief 提供统一的内存管理、退出机制、错误信息管理机制（实现）
 * @version 0.1
 * @date 2022-11-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "xtfs_manage.h"

#ifdef FEATURE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

void **pool = NULL;
int pool_top = -1;

void xtfs_exit(int ret) {
    int i;
    for (i = 0; i < pool_top; i++) {
        free(pool[i]);
    }
    exit(ret);
}

void *xtfs_malloc(size_t size) {
    #ifdef DEBUG
    assert( pool );
    #endif
    if (pool == NULL) {
        printf("XTFS_MANAGE NOT INIT!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    void *t = malloc(size);
    pool[++pool_top] = t;
    return t;
}
