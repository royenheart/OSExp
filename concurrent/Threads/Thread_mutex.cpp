#include <asm-generic/errno.h>
#include <cstddef>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <string>

/**
 * Using Mutex
 */

#define SLEEP_TIME 1

using namespace std;

typedef struct threadAttr threadAttr;
struct threadAttr {
    string name;
    int eatNum;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int food = 20;

void* thread(void* attr) {
    threadAttr myAttr = *(threadAttr*)attr;
    string myname = myAttr.name;
    int eatNum = myAttr.eatNum;
    while (food > 5) {
        sleep(SLEEP_TIME);
        pthread_mutex_lock(&mutex);
        if (food < eatNum) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        food -= eatNum;
        cout << myname << " Eat " << eatNum << " food." << endl << "Now left " << food << " food" << endl;
        pthread_mutex_unlock(&mutex);
    }
    cout << "Not enough food, " << myname << " dies" << endl;
    return ((void*)0);
}

int main(int argc, char* argv[]) {
    void* tret1, *tret2;
    // Due to threads running concurrently, can't assign their return to the same vars
    // will Occur some erorrs
    int errCode1, errCode2;
    int ret1, ret2;
    pthread_t h1, h2;
    threadAttr ta1;
    ta1.name = "thread1";
    ta1.eatNum = 6;
    threadAttr ta2;
    ta2.name = "thread2";
    ta2.eatNum = 5;
    if ((ret1 = pthread_create(&h1, NULL, thread, (void*)&ta1)) != 0) {
        cerr << "Thread Create Error" << endl;
        exit(EXIT_FAILURE);
    }
    if ((ret2 = pthread_create(&h2, NULL, thread, (void*)&ta2)) != 0) {
        cerr << "Thread Create Error!" << endl;
        exit(EXIT_FAILURE);
    }
    errCode1 = pthread_join(h1, &tret1);
    if (errCode1 > 0) {
        cerr << "h1 Error occur: " << errCode1 << endl;
        exit(EXIT_FAILURE);
    }
    errCode2 = pthread_join(h2, &tret2);
    if (errCode2 > 0) {
        cerr << "h2 Error occur: " << errCode2 << endl;
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
