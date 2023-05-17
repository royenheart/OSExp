#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child process Hello!\n");
        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        wait(NULL);
        printf("Parent process wait for child to finish\n");
        printf("Parent Hello");
    } else {
        printf("Child fork failed!\n");
        exit(EXIT_FAILURE);
    }
}