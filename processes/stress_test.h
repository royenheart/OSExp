#pragma once

#include <cblas.h>
#include <stdlib.h>

// 计算密集型函数
void matrix_cal(int, int, int);

void matrix_cal(int m, int n, int k) {
    double *A = NULL, *B = NULL, *C = NULL;

    A = (double *)malloc(m * k * sizeof(double));
    B = (double *)malloc(k * n * sizeof(double));
    C = (double *)malloc(m * n * sizeof(double));

    for (int i = 0; i < m * k; i++) {
        A[i] = (double)rand() / (double)RAND_MAX;
    }

    for (int i = 0; i < k * n; i++) {
        B[i] = (double)rand() / (double)RAND_MAX;
    }

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, 1.0, A, k,
                B, n, 0.0, C, n);

    free(A);
    free(B);
    free(C);
}