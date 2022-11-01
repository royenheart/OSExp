#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

pid_t wait(int* stat_loc);
void perror(const char* s);

int errno;
int global;

int main(int argc, char* argv) {
    int local = 0;
    int i;
    pid_t child;
    if ((child = fork()) == -1) {
        printf("Fork Error.\n");
    }
    if (child == 0) {
        printf("Now it is in child process.\n");
        if (execl("test", "test", NULL) == -1) {
            perror("Error in child process");
        }
        global = local + 2;
        exit(0);
    }
    printf("Now it is in parent process.\n");
    for (i = 0; i < 10; i++) {
        sleep(2);
        printf("Parent loop:%d\n",  i);
        if (i == 2) {
            if ((child = fork()) == -1) {
                printf("Fork Error.\n");
            }
            if (child == 0) {
                printf("Now it is in child process.\n");
                if (execl("test", "test", NULL) == -1) {
                    perror("Error in child process.\n");
                }
                global = local + 2;
                exit(0);
            }
        }
        if (i == 3) {
            pid_t temp;
            temp = wait(NULL);
            printf("Child process ID: %d\n", temp);
        }
    }
    global = local + 1;
    printf("Parent process is end, the local is %d, the global is %d.\n", local, global);
    exit(0);
}
