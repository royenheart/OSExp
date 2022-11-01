#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv) {
#ifdef _WIN32
    const char sys[] = "Windows system";
#elif defined(__linux__)
    const char sys[] = "Linux system";
#else
    const char sys[] = "Other system";
#endif

    printf("%s: char - %d\n", sys, sizeof(char));
    printf("%s: short - %d\n", sys, sizeof(short));
    printf("%s: int - %d\n", sys, sizeof(int));
    printf("%s: long - %d\n", sys, sizeof(long));
    printf("%s: long int - %d\n", sys, sizeof(long int));
    printf("%s: float - %d\n", sys, sizeof(float));
    printf("%s: double - %d\n", sys, sizeof(double));
}
