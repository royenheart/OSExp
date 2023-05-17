#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    pid_t me = getpid();
    pid_t father = getppid();

    printf("Child print from pid %d, father is %d\n", me, father);
    return 0;
}