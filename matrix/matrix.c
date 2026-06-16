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
}

int mfree(matrix_t** m) {
    if (m == NULL || *m == NULL) return MAT_ERROR_NULL_POINTER;

    if((*m)->data != NULL) {
        free((*m)->data);
    }; 
    
    free(*m);

    return MAT_SUCCESS;
}

int mprint(const matrix_t* m) {
    if (m == NULL) return MAT_ERROR_NULL_POINTER;
    if (m->data == NULL) return MAT_ERROR_NULL_DATA_POINTER;

    const float* data = m->data;
    const int rows = m->rows;
    const int cols = m->columns;

    // TODO: Move this to SIMD-optimized pointer increment
    for (int i=0; i < cols; i++) {
        for (int j=0; j < cols; j++) {
            printf("%f ", *(data+(i*cols)+j));
        };
        printf("\n");
    };
    printf("\n");

    return MAT_SUCCESS;
}

int mwrite(matrix_t* m, int row, int col, float val) {
    if (m == NULL) return MAT_ERROR_NULL_POINTER;
    if (col < 0 || row <0 || col >= m->columns || row >= m->rows) return MAT_ERROR_OUT_OF_BOUNDS;
    if (m->data == NULL) return MAT_ERROR_NULL_DATA_POINTER;

    *(m->data+(row*m->columns)+col) = val;

    return MAT_SUCCESS;
}

int mread(const matrix_t* m, int row, int col, float* res) {
    if (m == NULL) return MAT_ERROR_NULL_POINTER;
    if (col < 0 || row < 0 || col >= m->columns || row >= m->rows) return MAT_ERROR_OUT_OF_BOUNDS;
    if (m->data == NULL) return MAT_ERROR_NULL_DATA_POINTER;

    *res = *(m->data+(row*m->columns)+col);

    return MAT_SUCCESS;
}

int mtpose(const matrix_t* m, matrix_t* m_out) {
    if (m == NULL || m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (m->data == NULL || m_out->data == NULL) return MAT_ERROR_NULL_DATA_POINTER;
    if (m->data == m_out->data) return MAT_ALIASING_NOT_ALLOWED;
    if (m->rows != m_out->columns || m->columns != m_out->rows) return MAT_ERROR_DIMENSION_MISMATCH;

    const float* restrict src = m->data;
    float* restrict dst = m_out->data;
    const int src_rows = m->rows;
    const int src_cols = m->columns;
    const int dst_cols = m_out->columns;

    for (int i=0; i < src_rows; i++) {
        for (int j=0; j < src_cols; j++) {
            dst[j*dst_cols+i] = src[i*src_cols+j];
        };
    };

    return MAT_SUCCESS;
}

int madd(const matrix_t* a, const matrix_t* b, matrix_t* m_out) {
    if (a == NULL || b == NULL || m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (a->columns != b->columns ||
        a->rows != b->rows ||
        m_out->columns != a->columns ||
        m_out->rows != a->rows)
        return MAT_ERROR_DIMENSION_MISMATCH;

    // Look into pointer incremental operation here for SIMD optimization
    for (int i=0; i < a->rows; i++) {
        for (int j=0; j < a->columns; j++) {
            *(m_out->data+(i*m_out->columns)+j) = *(a->data+(i*a->columns)+j) + (*(b->data+(i*b->columns)+j));
        };
    };

    return MAT_SUCCESS;
}

int mmul_scalar(const matrix_t* m, float s, matrix_t* m_out) {
    if (m == NULL || m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (m->data == NULL || m_out->data == NULL) return MAT_ERROR_NULL_DATA_POINTER;
    if (m->data == m_out->data) return MAT_ALIASING_NOT_ALLOWED;

    const float* restrict src = m->data;
    float* restrict dst = m_out->data;
    const int src_rows = m->rows;
    const int src_cols = m->columns;
    const int dst_rows = m_out->rows;
    const int dst_cols = m_out->columns;

    for (int i=0; i < src_rows; i++) {
        for (int j=0; j < src_cols; j++) {
            dst[i*dst_cols+j] = src[i*src_cols+j]*s;
        };
    };

    return MAT_SUCCESS;
}

static int mmul_small(const matrix_t* a, const matrix_t* b, matrix_t* m_out) {
    const float* restrict src_a = a->data;
    const float* restrict src_b = b->data;
    float* restrict dst = m_out->data;
    const int src_a_rows = a->rows;
    const int src_a_cols = a->columns;
    const int src_b_rows = b->rows;
    const int src_b_cols = b->columns;
    const int dst_rows = m_out->rows;
    const int dst_cols = m_out->columns;


    for(int i=0; i < src_a_rows; i++) {
        const int offset = i*src_a_cols;
        for(int j=0; j < src_b_cols; j++) {
            float sum = 0.00000f;
            for (int k=0; k < src_a_cols; k++) {
                sum += src_a[offset+k] * src_b[k*src_b_cols+j] ;
            };
            dst[i*dst_cols+j] = sum;
        };
    };
    return MAT_SUCCESS;
}

static int mmul_tpose(const matrix_t* a, const matrix_t* b, matrix_t* m_out) {
    // Transpose the matrix to improve cache locality
    matrix_t *b_t;
    matrix_status_t res = mnew(b->columns, b->rows, &b_t);
    if (res != MAT_SUCCESS) return MAT_ERROR_ALLOCATION_FAILED;
    res = mtpose(b, b_t);
    if (res != MAT_SUCCESS){
        mfree(&b_t);
        return res;
    };

    const float* restrict src_a = a->data;
    const float* restrict src_b = b_t->data;
    float* restrict dst = m_out->data;
    const int src_a_rows = a->rows;
    const int src_a_cols = a->columns;
    const int src_b_rows = b_t->rows;
    const int src_b_cols = b_t->columns;
    const int dst_rows = m_out->rows;
    const int dst_cols = m_out->columns;

    // Begin multiplication using the transposed matrix
    for (int i=0; i < src_a_rows; i++) {
        const int i_ofst = i*src_a_cols;
        for (int j=0; j < src_b_cols; j++) {
            const int j_ofst = j*src_b_cols;
            float sum = 0.00000f;
            for (int k=0; k < src_a_cols; k++) {
                sum += src_a[i_ofst+k] * src_b[j_ofst+k];
            };
            dst[i*dst_cols+j] = sum;
        };
    };
    mfree(&b_t);
    return MAT_SUCCESS;
}

static int mmul_lt(const matrix_t* a, const matrix_t* b, matrix_t* m_out) {
    //TODO 3
    return MAT_SUCCESS;
}

int mmul(const matrix_t* a, const matrix_t* b, matrix_t* m_out) {
    if (a == NULL || b == NULL || m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (m_out->data == a->data || m_out->data == b->data) return MAT_ALIASING_NOT_ALLOWED;
    if (a->columns != b->rows ||
        m_out->rows != a ->rows ||
        m_out->columns != b-> columns)
        return MAT_ERROR_DIMENSION_MISMATCH;

    const int tpose_threshold = 32;
    const int lt_threshold = 256;

    if ((a->rows > lt_threshold && a->columns > lt_threshold) || (b->rows > lt_threshold && b->columns > lt_threshold)) return mmul_lt(a, b, m_out);
    if (b->columns > tpose_threshold) return mmul_tpose(a, b, m_out);
    return mmul_small(a, b, m_out);
}

