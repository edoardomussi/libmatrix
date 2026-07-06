#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "../include/matrix.h"
#include "../include/matrix_internal.h"


static double get_time_seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

static void populate_matrix(matrix_s* m) {
    for (size_t r = 0; r < m->rows; r++) {
        for (size_t c = 0; c < m->columns; c++) {
            m->data[r*m->stride+c] = 0.25f + ((float)rand() / (float)RAND_MAX);
        }
    }
}

int main() {
    const size_t N = 32;
    const int ITERATIONS = 10000;
    int status = 0;

    matrix_s *a, *b, *c;
    status = mnew(N, N, &a);
    if (status != 0) return 1;
    status = mnew(N, N, &b);
    if (status != 0) return 1;
    status = mnew(N, N, &c);
    if (status != 0) return 1;

    populate_matrix(a);
    populate_matrix(b);

    printf("Benchmarking mmul_small (AVX2) on %zux%zu matrices...\n", N, N);

    mmul_lt(a, b, c);

    double start_time = get_time_seconds();

    for (int i = 0; i < ITERATIONS; i++) {
        status = mmul_lt(a, b, c);
        // if (status != 0) printf("mmul failed on iteration %i", i);
    }

    double end_time = get_time_seconds();
    double total_time = end_time - start_time;
    double avg_time = total_time / ITERATIONS;

    volatile float sum = c->data[0];

    double total_flops = 2.0 * (double)N * (double)N * (double)N;
    double gflops = (total_flops / 1000000000.0) / avg_time;

    printf("Iterations: %d | Avg time: %.7f seconds | Throughput: %.2f GFLOPS | Sanity sum: %f\n", ITERATIONS, avg_time, gflops, sum);

    mfree(&a);
    mfree(&b);
    mfree(&c);

    return 0;
}
