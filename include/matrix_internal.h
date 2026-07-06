#pragma once
#include "matrix.h"

#ifdef MATRIX_TEST_BUILD
    #define MATRIX_INTERNAL
#else
    #define MATRIX_INTERNAL static
#endif

MATRIX_INTERNAL int mmul_small(const matrix_s* a, const matrix_s* b, matrix_s* m_out);
MATRIX_INTERNAL int mmul_tpose(const matrix_s* a, const matrix_s* b, matrix_s* m_out);
MATRIX_INTERNAL int mmul_lt(const matrix_s* a, const matrix_s* b, matrix_s* m_out);
