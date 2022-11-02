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

    printf("%s: char - %ld\n", sys, sizeof(char));
    printf("%s: short - %ld\n", sys, sizeof(short));
    printf("%s: int - %ld\n", sys, sizeof(int));
    printf("%s: long - %ld\n", sys, sizeof(long));
    printf("%s: long int - %ld\n", sys, sizeof(long int));
    printf("%s: float - %ld\n", sys, sizeof(float));
    printf("%s: double - %ld\n", sys, sizeof(double));
}
