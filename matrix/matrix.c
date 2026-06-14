#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int rows;
    int columns;
    float* data;
} matrix_t;

// Allocate a new matrix
matrix_t* mnew(int r, int c) {
    matrix_t* m = malloc(sizeof(matrix_t));
    m->rows = r;
    m->columns = c;
    m->data = malloc(sizeof(float)*r*c);
    return m;
};

// Free the matrix
void mfree(matrix_t* m) {
    free(m->data);
    free(m);
};

// Print the matrix
void mprint(matrix_t* m) {
    for (int i=0; i < m->rows; i++) {
        for (int j=0; j < m->columns; j++) {
            printf("%f", *(m->data+(i*m->columns)+j));
        }
        printf("\n");
    }
    return;
};

// Write a value to the matrix
int mwrite(matrix_t* m, int row, int col, float val) {
    if ((col-1) > m->columns || (row-1) > m->rows) return 1;
    *(m->data+((row-1)*m->columns)+col-1) = val;
    return 0;
};

// Read a value from the matrix
float mread(matrix_t* m, int row, int col) {
    if ((col-1) > m->columns || (row-1) > m->rows) return (float)1;
    return *(m->data+((row-1)*m->columns)+col-1);
};

// Add two matrices together
bool madd(matrix_t* a, matrix_t* b) {
    if (a->columns != b->columns || a->rows != b->rows) return true;

    for (int i=0; i < a->rows; i++) {
        for (int j=0; j < a->columns; j++) {
            *(a->data+(i*a->columns)+j) += *(b->data+(i*a->columns)+j);
        }
    }
    return false;
};

// Multiply a matrix by a scalar
void mmul_scalar(matrix_t* m, float s) {
    for (int i=0; i < m->rows; i++) {
        for (int j=0; j < m->columns; j++) {
            *(m->data+(i*m->columns)+j) *= s;
        }
    }
    return;
};

// Multiply two matrices together
bool mmul(matrix_t* a, matrix_t* b) {
    if (a->columns != b->rows) return true;

    // TODO: implement proper matrix multiplication
    return false;
}

