#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include "matrix.h"

#include <unistd.h>

bool float_eq(float a, float b) {
    return fabs(a-b) < 0.0001f;
}

bool matrix_t_eq(const matrix_t* a, const matrix_t* b) {
    if (a->rows != b->rows) return false;
    if (a->columns != b->columns) return false;
    for (int i=0; i < (a->rows*a->columns); i++)
        if (!float_eq(*(a->data+i), *(b->data+i))) return false;
    return true;
}

void test_allocation() {
    matrix_t *m;
    mnew(32768, 32768, &m);

    assert(m != NULL);

    assert(m->rows == 32768);
    assert(m->columns == 32768);

    assert(m->data != NULL);
    for (int i=0; i < (m->rows*m->columns); i++)
        assert(*(m->data+i) == 0.0f);

    sleep(10);
    mfree(&m);
    printf("Allocation tests passed!\n");
}

void test_write() {
    matrix_t *m, *n;
    mnew(2, 2, &m);
    mnew(2, 2, &n);
    assert(matrix_t_eq(m, n));

    int res = mwrite(m, 0, 0, 5.0f);
    assert(res == MAT_SUCCESS);
    assert(!matrix_t_eq(m, n));

    mfree(&m);
    mfree(&n);
    printf("Write test passed!\n");
}

void test_read() {
    matrix_t *m, *n;
    mnew(2, 2, &m);
    mnew(2, 2, &n);
    assert(matrix_t_eq(m, n));

    mwrite(m, 0, 0, 5.0f);
    mwrite(n, 0, 0, 5.0f);
    float a;
    int res = mread(m, 0, 0, &a);
    assert(res == MAT_SUCCESS);
    assert(matrix_t_eq(m, n));
    assert(float_eq(a, 5.0f));

    mfree(&m);
    mfree(&n);
    printf("Read test passed!\n");
}

void test_transposition() {
    matrix_t *m, *m_out;
    mnew(3, 5, &m);
    mnew(5, 3, &m_out);
    assert(m != NULL);
    assert(m_out != NULL);

    mwrite(m, 0, 0, 5.0f);
    mwrite(m, 0, 1, 5.0f);
    int status = mtpose(m, m_out);

    assert(status == MAT_SUCCESS);

    float res;
    mread(m_out, 1, 0, &res);
    assert(float_eq(res, 5.0f));

    mfree(&m);
    mfree(&m_out);
    printf("Transposition test passed!\n");
}

void test_addition() {
    matrix_t *m, *n, *m_out;
    mnew(2, 3, &m);
    mnew(2, 3, &n);
    mnew(2, 3, &m_out);

    mwrite(m, 0, 0, 5.0f);
    mwrite(n, 0, 0, 5.0f);
    assert(matrix_t_eq(m, n));

    int status = madd(m, n, m_out);
    assert(status == MAT_SUCCESS);
    assert(!matrix_t_eq(m, m_out));

    float res;
    mread(m_out, 0, 0, &res);
    assert(float_eq(res, 10.0f));

    mfree(&m);
    mfree(&n);
    mfree(&m_out);
    printf("Addition test passed!\n");
}

void test_mmul_scalar() {
    matrix_t *m, *m_out;
    float s = 5.0f;
    mnew(2, 2, &m);
    mnew(2, 2, &m_out);
    assert(matrix_t_eq(m, m_out));

    mwrite(m, 0, 0, 5.0f);
    assert(!matrix_t_eq(m, m_out));

    int res = mmul_scalar(m, s, m_out);
    assert(res == MAT_SUCCESS);

    float f;
    mread(m_out, 0, 0, &f);
    assert(float_eq(f, 25.0f));

    mfree(&m);
    mfree(&m_out);
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
