#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"

#include <unistd.h>

bool float_eq(float a, float b) {
    return fabs(a-b) < 0.0001f;
}

void test_allocation() {
    matrix_s *m;
    matrix_status_s res;

    // Allocate the matrix
    res = mnew(32768, 32768, &m);
    assert(res == MAT_SUCCESS);
    assert(m != NULL);
    assert(m->rows == 32768);
    assert(m->columns == 32768);
    assert(m->data != NULL);

    const int rows = m->rows;
    const int cols = m->columns;
    const float* data = m->data;

    // Check the data section was correctly initialized and zeroed
    for (int i=0; i < (rows*cols); i++)
        assert(data[i] == 0.0f);

    // Free the matrix
    res = mfree(&m);
    assert(res == MAT_SUCCESS);
    printf("Allocation tests passed!\n");
}

void test_write() {
    matrix_s *m, *n;
    matrix_status_s res;
    bool e;

    // Allocate the matrices
    res = mnew(2, 2, &m);
    assert(res == MAT_SUCCESS);
    res = mnew(2, 2, &n);
    assert(res == MAT_SUCCESS);
    res = meq(m, n, &e);
    assert(res == MAT_SUCCESS);
    assert(e);

    // Write to Matrix M
    res = mwrite(m, 0, 0, 5.0f);
    assert(res == MAT_SUCCESS);

    // Check M==N
    res = meq(m, n, &e);
    assert(res == MAT_SUCCESS);
    assert(!e);

    // Free the matrices
    res = mfree(&m);
    assert(res == MAT_SUCCESS);
    res = mfree(&n);
    assert(res == MAT_SUCCESS);
    printf("Write test passed!\n");
}

void test_read() {
    matrix_s *m, *n;
    matrix_status_s res;
    bool e;

    // Allocate the matrices
    res = mnew(2, 2, &m);
    assert(res == MAT_SUCCESS);
    res = mnew(2, 2, &n);
    assert(res == MAT_SUCCESS);
    res = meq(m, n, &e);
    assert(res == MAT_SUCCESS);
    assert(e);

    // Write to the matrices
    res = mwrite(m, 0, 0, 5.0f);
    assert(res == MAT_SUCCESS);
    res = mwrite(n, 0, 0, 5.0f);
    assert(res == MAT_SUCCESS);

    // Test for the written value
    float a;
    res = mread(m, 0, 0, &a);
    assert(res == MAT_SUCCESS);
    res = meq(m, n, &e);
    assert(res == MAT_SUCCESS);
    assert(e);
    assert(float_eq(a, 5.0f));

    // Free the matrices
    res = mfree(&m);
    assert(res == MAT_SUCCESS);
    res = mfree(&n);
    assert(res == MAT_SUCCESS);
    printf("Read test passed!\n");
}

void test_transposition() {
    matrix_s *m, *m_out;
    matrix_status_s res;

    // Allocate matrix
    res = mnew(3, 5, &m);
    assert(res == MAT_SUCCESS);
    res = mnew(5, 3, &m_out);
    assert(res == MAT_SUCCESS);
    assert(m != NULL && m->data != NULL);
    assert(m_out != NULL && m->data != NULL);

    //Write to the source matrix
    res = mwrite(m, 0, 0, 5.0f);
    assert(res == MAT_SUCCESS);
    res = mwrite(m, 0, 1, 5.0f);
    assert(res == MAT_SUCCESS);
    
    // Perform the transposition
    res = mtpose(m, m_out);
    assert(res == MAT_SUCCESS);

    // Check the value was correctly transposed
    float a;
    res = mread(m_out, 1, 0, &a);
    assert(res == MAT_SUCCESS);
    assert(float_eq(a, 5.0f));

    // Free the matrices
    res = mfree(&m);
    assert(res == MAT_SUCCESS);
    res = mfree(&m_out);
    assert(res == MAT_SUCCESS);
    printf("Transposition test passed!\n");
}

void test_addition() {
    matrix_s *m, *n, *m_out;
    matrix_status_s res;
    bool e;

    // Allocate the matrices
    res = mnew(2, 3, &m);
    assert(res == MAT_SUCCESS);
    res = mnew(2, 3, &n);
    assert(res == MAT_SUCCESS);
    res = mnew(2, 3, &m_out);
    assert(res == MAT_SUCCESS);

    // Write to the source matrices
    res = mwrite(m, 0, 0, 5.0f);
    assert(res == MAT_SUCCESS);
    res = mwrite(n, 0, 0, 5.0f);
    assert(res == MAT_SUCCESS);
    res = meq(m, n, &e);
    assert(res == MAT_SUCCESS);
    assert(e);

    // Perform the addition
    res = madd(m, n, m_out);
    assert(res == MAT_SUCCESS);
    res = meq(m, m_out, &e);
    assert(!e);

    // Check the addition was correct
    float a;
    res = mread(m_out, 0, 0, &a);
    assert(res == MAT_SUCCESS);
    assert(float_eq(a, 10.0f));

    // Free the matrices
    res = mfree(&m);
    assert(res == MAT_SUCCESS);
    res = mfree(&n);
    assert(res == MAT_SUCCESS);
    res = mfree(&m_out);
    assert(res == MAT_SUCCESS);
    printf("Addition test passed!\n");
}

void test_mmul_scalar() {
    matrix_s *m, *m_out;
    matrix_status_s res;
    bool e;
    float s = 5.0f;

    // Allocate the matrices
    res = mnew(2, 2, &m);
    assert(res == MAT_SUCCESS);
    res = mnew(2, 2, &m_out);
    assert(res == MAT_SUCCESS);
    res = meq(m, m_out, &e);
    assert(res == MAT_SUCCESS);
    assert(e);

    // Write to the source matrix
    res = mwrite(m, 0, 0, 5.0f);
    assert(res == MAT_SUCCESS);
    res = meq(m, m_out, &e);
    assert(res == MAT_SUCCESS);
    assert(!e);

    // Perform scalar multiplication
    res = mmul_scalar(m, s, m_out);
    assert(res == MAT_SUCCESS);

    // Check the multiplication was correct
    float f;
    res = mread(m_out, 0, 0, &f);
    assert(res == MAT_SUCCESS);
    assert(float_eq(f, 25.0f));

    // Free the matrices
    res = mfree(&m);
    assert(res == MAT_SUCCESS);
    res = mfree(&m_out);
    assert(res == MAT_SUCCESS);
    printf("Multiplication matrix-scalar tests passed!\n");
}

void test_mmul(){
    // TODO: implement mmul() test
}

int main() {
    printf("Running tests...\n\n");

    test_allocation();
    test_write();
    test_read();
    test_transposition();
    test_addition();
    test_mmul_scalar();

    printf("\nAll tests passed!\n");
    return 0;
}
