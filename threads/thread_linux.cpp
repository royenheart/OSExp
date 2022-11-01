#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>

#ifndef SUB_THREAD_SLEEP
#define SUB_THREAD_SLEEP 2
#endif

#ifndef SUB_THREAD_LOOP
#define SUB_THREAD_LOOP 8
#endif

#ifndef MAIN_THREAD_SLEEP
#define MAIN_THREAD_SLEEP 1
#endif

#ifndef MAIN_THREAD_LOOP
#define MAIN_THREAD_LOOP 6
#endif

#define WHO_MAIN 1
#define WHO_THREAD 2

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int who = WHO_MAIN;

void* SubThread(void*) {
    int i;
    for (i = 0; i < SUB_THREAD_LOOP; i++) {
        pthread_mutex_lock(&mutex);
        while (who != WHO_THREAD) {
            pthread_cond_wait(&c, &mutex);
        }
        cout << "SubThread" << endl;
        pthread_mutex_unlock(&mutex);
        sleep(SUB_THREAD_SLEEP);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    cout << "Create Thread" << endl;
    pthread_t hThread;
    int ret;
    if ((ret = pthread_create(&hThread, NULL, SubThread, NULL)) != 0) {
        cout << "Create Thread Error" << endl;
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; i < MAIN_THREAD_LOOP; i++) {
        cout << "Main Thread: " << i << endl;
#if MAIN_THREAD_LOOP > 3
        if (i == 1) {
            pthread_mutex_lock(&mutex);
        }
        if (i == 3) {
            who = WHO_THREAD;
            pthread_cond_signal(&c);
            pthread_mutex_unlock(&mutex);
        }
#endif
        sleep(MAIN_THREAD_SLEEP);
    }
#if MAIN_THREAD_LOOP < 4
    pthread_mutex_lock(&mutex);
    who = WHO_THREAD;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&mutex);
#endif
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}