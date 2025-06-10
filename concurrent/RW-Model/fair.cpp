/**
 * @file fair.c
 * @author royenheart (royenheart@outlook.com)
 * @brief 公平机制的读者写者模型信号量实现
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#define MAX_LOOP 4
#define MIN_SLEEP_TIME 1
#define MAX_SLEEP_TIME 5
#define RWTERS 5
#define RAND_TIME(x, y) rand() % y + x;

using namespace std;

typedef struct thread_header {
    string name;
    int sleep_time;
} thread_header;

string book = "NULL";
int writeCount = 0, readCount = 0;
sem_t rsem, wsem;
sem_t x, y;

void init() {
    sem_init(&rsem, 0, 1);
    sem_init(&wsem, 0, 1);
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);
}

void writeBook(string content, string who) {
    cout << who << " edit the book to: " << content << endl;
    book = content;
}

void readBook(string who) {
    cout << who << " is reading. read: " << book << endl;
}

void *read(void *header) {
    thread_header *myheader = (thread_header *)header;
    int i;
    for (i = 0; i < MAX_LOOP; i++) {
        sleep(myheader->sleep_time);
        sem_wait(&rsem);
        sem_wait(&x);
        if (readCount == 0) {
            sem_wait(&wsem);
        }
        readCount++;
        sem_post(&x);
        sem_post(&rsem);

        readBook(myheader->name);

        sem_wait(&x);
        readCount--;
        if (readCount == 0) {
            sem_post(&wsem);
        }
        sem_post(&x);
    }
    return ((void *)0);
}

void *write(void *header) {
    thread_header *myheader = (thread_header *)header;
    int i;
    for (i = 0; i < MAX_LOOP; i++) {
        sleep(myheader->sleep_time);
        sem_wait(&y);
        if (writeCount == 0) {
            sem_wait(&rsem);
        }
        writeCount++;
        sem_post(&y);

        sem_wait(&wsem);
        stringstream cont;
        cont << myheader->name << ":HelloWorld";
        writeBook(cont.str(), myheader->name);
        sem_post(&wsem);

        sem_wait(&y);
        writeCount--;
        if (writeCount == 0) {
            sem_post(&rsem);
        }
        sem_post(&y);
    }
    return ((void *)0);
}

int main(int argc, char *argv[]) {
    init();

    int i;
    pthread_t readers[RWTERS];
    pthread_t writers[RWTERS];
    stringstream reader[RWTERS], writer[RWTERS];
    thread_header readH[RWTERS], writeH[RWTERS];
    srand((unsigned)time(NULL));

    for (i = 0; i < RWTERS; i++) {
        reader[i] << "reader" << i;
        writer[i] << "writer" << i;
        readH[i].name = reader[i].str();
        readH[i].sleep_time = RAND_TIME(MIN_SLEEP_TIME, MAX_SLEEP_TIME);
        writeH[i].name = writer[i].str();
        writeH[i].sleep_time = RAND_TIME(MIN_SLEEP_TIME, MAX_SLEEP_TIME);
        pthread_create(&readers[i], NULL, read, (void *)&readH[i]);
        pthread_create(&writers[i], NULL, write, (void *)&writeH[i]);
    }

    for (i = 0; i < RWTERS; i++) {
        pthread_join(readers[i], NULL);
        pthread_join(writers[i], NULL);
    }

    return EXIT_SUCCESS;
}