#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "matrix.h"

int mnew(int rows, int cols, matrix_t** m_out) {
    // Allocate the outer structure
    *m_out = malloc(sizeof(matrix_t));
    if ((*m_out) == NULL) return MAT_ERROR_ALLOCATION_FAILED;

    // Initialize the inner values
    (*m_out)->rows = rows;
    (*m_out)->columns = cols;
    // Allocate the inner structure
    (*m_out)->data = calloc(rows*cols, sizeof(float));
    if ((*m_out)->data == NULL) {
        free(*m_out);
        *m_out = NULL;
        return MAT_ERROR_ALLOCATION_FAILED;
    };

    return MAT_SUCCESS;
};

int mfree(matrix_t** m) {
    if (*m == NULL || (*m)->data == NULL) return MAT_ERROR_NULL_POINTER;
    
    free((*m)->data);
    free(*m);

    return MAT_SUCCESS;
};

int mprint(const matrix_t* m) {
    if (m == NULL) return MAT_ERROR_NULL_POINTER;

    for (int i=0; i < m->rows; i++) {
        for (int j=0; j < m->columns; j++) {
            printf("%f ", *(m->data+(i*m->columns)+j));
        };
        printf("\n");
    };
    printf("\n");

    return MAT_SUCCESS;
};

int mwrite(matrix_t** m, int row, int col, float val) {
    if (*m == NULL) return MAT_ERROR_NULL_POINTER;
    if (col < 0 || row <0 || col >= (*m)->columns || row >= (*m)->rows) return MAT_ERROR_OUT_OF_BOUNDS;

    *((*m)->data+(row*(*m)->columns)+col) = val;

    return MAT_SUCCESS;
};

int mread(const matrix_t* m, int row, int col, float* res) {
    if (m == NULL) return MAT_ERROR_NULL_POINTER;
    if (col < 0 || row < 0 || col > m->columns || row > m->rows) return MAT_ERROR_OUT_OF_BOUNDS;

    *res = *(m->data+(row*m->columns)+col);

    return MAT_SUCCESS;
};

int mtpose(const matrix_t* m, matrix_t** m_out) {
    if (m == NULL || (*m_out) == NULL) return MAT_ERROR_NULL_POINTER;
    if (m->rows != (*m_out)->columns || m->columns != (*m_out)->rows) return MAT_ERROR_DIMENSION_MISMATCH;

    for (int i=0; i<m->rows; i++) {
        for (int j=0; j<m->columns; j++) {
            *((*m_out)->data+(j*(*m_out)->columns)+i) = *(m->data+(i*m->columns)+j);
        };
    };

    return MAT_SUCCESS;
};

int madd(const matrix_t* a, const matrix_t* b, matrix_t** m_out) {
    if (a == NULL || b == NULL || *m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (a->columns != b->columns ||
        a->rows != b->rows ||
        (*m_out)->columns != a->columns ||
        (*m_out)->rows != a->rows)
        return MAT_ERROR_DIMENSION_MISMATCH;

    for (int i=0; i < a->rows; i++) {
        for (int j=0; j < a->columns; j++) {
            *((*m_out)->data+(i*(*m_out)->columns)+j) = *(a->data+(i*a->columns)+j) + (*(b->data+(i*b->columns)+j));
        };
    };

    return MAT_SUCCESS;
};

int mmul_scalar(const matrix_t* m, float s, matrix_t** m_out) {
    if (m == NULL || *m_out == NULL) return MAT_ERROR_NULL_POINTER;

    for (int i=0; i < m->rows; i++) {
        for (int j=0; j < m->columns; j++) {
            *((*m_out)->data+(i*(*m_out)->columns)+j) = *(m->data+(i*m->columns)+j)*s;
        };
    };

    return MAT_SUCCESS;
};

int mmul(const matrix_t* a, const matrix_t* b, matrix_t** m_out) {
    if (a == NULL || b == NULL || *m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (a->columns != b->rows ||
        (*m_out)->rows != a ->rows ||
        (*m_out)->columns != b-> columns)
        return MAT_ERROR_DIMENSION_MISMATCH;

    for(int i=0; i < a->rows; i++) {
        for(int j=0; j < b->columns; j++) {
            for (int k=0; k < a->columns; k++) {
                *((*m_out)->data+(i*(*m_out)->columns)+j) += *(a->data+(i*a->columns)+k) * (*(b->data+(k*b->columns)+j)) ;
            }
        }
    }
    return MAT_SUCCESS;
}

