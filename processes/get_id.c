#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());
    printf("RUID: %d\n", getuid());
    printf("EUID: %d\n", geteuid());
    printf("RGID: %d\n", getgid());
    printf("EGID: %d\n", getegid());
    return 0;
}