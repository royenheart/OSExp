/**
 * @file select.cpp
 * @author royenheart (royenheart@outlook.com)
 * @brief 黑棋白棋并发选择（pthread互斥锁和条件变量实现）
 * @version 0.1
 * @date 2022-11-06
 *
 * @copyright Copyright (c) 2022
 *
 * 要求：
 * 一个盒子里混装了数量相等的黑白棋子，设进程P1和P2分别分拣出黑白棋子。
 * 1. 每个进程一次分拣一个，当一个进程在分拣时，其他进程不允许访问
 * 2. 一个进程执行后，必须交给另一个进程执行
 */

#include <pthread.h>
#include <semaphore.h>
#include <sys/timeb.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#define RAND_NUM(x, y) rand() % y + x;

typedef struct box box;
struct box {
    int black;
    int white;
};

box *BOX = NULL;
pthread_mutex_t res_mutex;
// 0: black ; 1 : white
pthread_cond_t cond_order;
int who = 0;
int blacks = 0;
int whites = 0;
// 0: black ; 1 : white
int check = -1;

void *pblack(void *args) {
    while (BOX->black > 0) {
        pthread_mutex_lock(&res_mutex);
        if (who == 1) {
            pthread_cond_wait(&cond_order, &res_mutex);
        }
        BOX->black--;
        blacks++;
        std::cout << "GET BLACK" << std::endl;
        if (check == 0) {
            std::clog << "NOT ARRAY RIGHT" << std::endl;
            return (void *)EXIT_FAILURE;
        }
        check = 0;
        who = 1;
        pthread_cond_signal(&cond_order);
        pthread_mutex_unlock(&res_mutex);
    }
    return (void *)0;
}

void *pwhite(void *args) {
    while (BOX->white > 0) {
        pthread_mutex_lock(&res_mutex);
        if (who == 0) {
            pthread_cond_wait(&cond_order, &res_mutex);
        }
        BOX->white--;
        whites++;
        std::cout << "GET WHITE" << std::endl;
        if (check == 1) {
            std::clog << "NOT ARRAY RIGHT" << std::endl;
            return (void *)EXIT_FAILURE;
        }
        check = 1;
        who = 0;
        pthread_cond_signal(&cond_order);
        pthread_mutex_unlock(&res_mutex);
    }
    return (void *)0;
}

int main(int argc, char *argv[]) {
    BOX = (box *)malloc(sizeof(box));

    struct timeb timeSeed;
    ftime(&timeSeed);
    srand(timeSeed.time * 1000 + timeSeed.millitm);

    int bnum, wnum;
    bnum = wnum = RAND_NUM(3, 10);
    BOX->black = bnum;
    BOX->white = wnum;
    std::cout << "BLACK and WHITE has: " << bnum << std::endl;

    res_mutex = PTHREAD_MUTEX_INITIALIZER;
    cond_order = PTHREAD_COND_INITIALIZER;

    pthread_t p1, p2;
    int ret1, ret2;
    pthread_create(&p1, NULL, pblack, NULL);
    pthread_create(&p2, NULL, pwhite, NULL);
    ret1 = pthread_join(p1, NULL);
    ret2 = pthread_join(p2, NULL);

    if (bnum == blacks && wnum == whites && ret1 + ret2 == 0) {
        std::cout << "SELECT PASSED!" << std::endl;
    } else {
        std::cout << "SELECT FAILED!" << std::endl;
    }

    pthread_mutex_destroy(&res_mutex);
    pthread_cond_destroy(&cond_order);
    free(BOX);

    return EXIT_SUCCESS;
}
