#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "xtfs_manage.h"

#ifdef FEATURE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

void** pool = NULL;
int pool_top = -1;

void xtfs_exit(int ret) {
    int i;
    for (i = 0; i < MAX_POOL_LENGTH; i++) {
        free(pool[i]);
    }
    exit(ret);
}

void* xtfs_malloc(size_t size) {
#ifdef DEBUG
    assert(pool);
#endif
    if (pool == NULL) {
        printf("XTFS_MANAGE NOT INIT!\n");
        xtfs_exit(EXIT_FAILURE);
    }
    void* t = malloc(size);
    pool[++pool_top] = t;
    return t;
}
