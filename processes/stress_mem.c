#include <stdio.h>
#include "stress_test.h"

int main(int argc, char* argv[]) {
    matrix_cal(3000, 4000, 9000);
    matrix_cal(3000, 3000, 3000);
    matrix_cal(3000, 8000, 1000);
    matrix_cal(1000, 2000, 3000);
    matrix_cal(300, 400, 200);
    matrix_cal(2000, 3000, 12000);
    matrix_cal(10, 20, 30);
}