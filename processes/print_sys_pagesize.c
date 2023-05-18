#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    long page_size = sysconf(_SC_PAGE_SIZE);
    printf("PAGE SIZE of SYS is %ld bytes\n", page_size);

    return 0;
}