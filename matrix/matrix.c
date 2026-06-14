#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "matrix.h"

matrix_t* mnew(int r, int c) {
    matrix_t* m = malloc(sizeof(matrix_t));
    m->rows = r;
    m->columns = c;
    m->data = calloc(r*c, sizeof(float));
    return m;
};

void mfree(matrix_t* m) {
    free(m->data);
    free(m);
};

void mprint(matrix_t* m) {
    for (int i=0; i < m->rows; i++) {
        for (int j=0; j < m->columns; j++) {
            printf("%f", *(m->data+(i*m->columns)+j));
        }
        printf("\n");
    }
    return;
};

int mwrite(matrix_t* m, int row, int col, float val) {
    if ((col-1) > m->columns || (row-1) > m->rows) return 1;
    *(m->data+((row-1)*m->columns)+col-1) = val;
    return 0;
};

float mread(matrix_t* m, int row, int col) {
    if ((col-1) > m->columns || (row-1) > m->rows) return (float)1;
    return *(m->data+((row-1)*m->columns)+col-1);
};

matrix_t* madd(matrix_t* a, matrix_t* b) {
    if (a->columns != b->columns || a->rows != b->rows) return NULL;

    matrix_t* m = mnew(a->rows, b->columns);

    for (int i=0; i < a->rows; i++) {
        for (int j=0; j < a->columns; j++) {
            *(m->data+(i*m->columns)+j) = *(a->data+(i*a->columns)+j) + (*(b->data+(i*b->columns)+j));
        }
    }
    return m;
};

void mmul_scalar(matrix_t* m, float s) {
    for (int i=0; i < m->rows; i++) {
        for (int j=0; j < m->columns; j++) {
            *(m->data+(i*m->columns)+j) *= s;
        }
    }
    return;
};

matrix_t* mmul(matrix_t* a, matrix_t* b) {
    if (a->columns != b->rows) return NULL;

    matrix_t* m = mnew(a->rows, b->columns);

    for(int i=0; i < a->rows; i++) {
        for(int j=0; j < b->columns; j++) {
            for (int k=0; k < a->columns; k++) {
                *(m->data+(i*m->columns)+j) += *(a->data+(i*a->columns)+k) * (*(b->data+(k*b->columns)+j)) ;
            }
        }
    }
    return m;
}

