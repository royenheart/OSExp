#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());
    printf("Real User ID: %d\n", getuid());
    printf("Effective User ID: %d\n", geteuid());
    printf("Real Group ID: %d\n", getgid());
    printf("Effective Group ID: %d\n", getegid());
    return 0;
}