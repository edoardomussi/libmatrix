#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <immintrin.h>
#include <xmmintrin.h>


#include "../include/matrix.h"
#include "../include/matrix_internal.h"

#define CACHE_THRESHOLD_BYTES (256 * 1024)
#define BLK_SIZE 32
#define TILE_SIZE 16


static inline bool float_eq(float a, float b) {
    return fabsf(a-b) < 0.00001f;
}

int mnew(int rows, int cols, matrix_s** m_out) {
    // Calculate stride
    size_t stride = (cols + (BLK_SIZE -1)) & ~(BLK_SIZE - 1);
    int is_pow2 = (stride > 0) & ((stride & (stride - 1)) == 0);
    stride += is_pow2 * BLK_SIZE;

    // Calculate the size of the data payload
    if (rows <= 0 || cols <= 0) return MAT_ERROR_INVALID_SIZE;
    size_t p_size = (size_t)rows * stride * sizeof(float);

    // Calculate the total size of the structure
    size_t t_size = sizeof(matrix_s) + p_size;

    if (m_out == NULL) return MAT_ERROR_NULL_POINTER;
    *m_out = aligned_alloc(BLK_SIZE, t_size);
    if (*m_out == NULL) return MAT_ERROR_ALLOCATION_FAILED;

    memset(*m_out, 0, t_size);
    (*m_out)->rows = rows;
    (*m_out)->columns = cols;
    (*m_out)->stride = stride;

    return MAT_SUCCESS;
}

int mfree(matrix_s** m) {
    if (m == NULL || *m == NULL) return MAT_ERROR_NULL_POINTER;
    free (*m);
    *m = NULL;
    return MAT_SUCCESS;
}

int mwrite(matrix_s* m, int row, int col, float val) {
    if (m == NULL) return MAT_ERROR_NULL_POINTER;
    if (col < 0 || row <0 || col >= m->columns || row >= m->rows) return MAT_ERROR_OUT_OF_BOUNDS;

    *(m->data + ((size_t)row * m->stride) + col) = val;

    return MAT_SUCCESS;
}

int mread(const matrix_s* m, int row, int col, float* res) {
    if (m == NULL) return MAT_ERROR_NULL_POINTER;
    if (col < 0 || row < 0 || col >= m->columns || row >= m->rows) return MAT_ERROR_OUT_OF_BOUNDS;

    *res = *(m->data + ((size_t)row * m->stride) + col);

    return MAT_SUCCESS;
}

int meq(const matrix_s* a, const matrix_s* b, bool* res) {
    if (a == NULL || b == NULL) return MAT_ERROR_NULL_POINTER;
    *res = false;
    if (a->rows != b->rows || a->columns != b->columns) return MAT_SUCCESS;

    const int rows = a->rows;
    const int cols = a->columns;
    const size_t a_stride = a->stride;
    const size_t b_stride = b->stride;
    const float* a_data = a->data;
    const float* b_data = b->data;

    for(int i=0; i < rows; i++) {
        const float* row_a = a_data + ((size_t)i * a_stride);
        const float* row_b = b_data + ((size_t)i * b_stride);
        for (int j=0; j < cols; j++) {
            if (!float_eq(row_a[j], row_b[j])) return MAT_SUCCESS;
        }
    }

    *res = true;
    return MAT_SUCCESS;
}

int mtpose(const matrix_s* m, matrix_s* m_out) {
    if (m == NULL || m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (m == m_out) return MAT_ALIASING_NOT_ALLOWED;
    if (m->rows != m_out->columns || m->columns != m_out->rows) return MAT_ERROR_DIMENSION_MISMATCH;

    const float* restrict src = m->data;
    float* restrict dst = m_out->data;
    const int src_rows = m->rows;
    const int src_cols = m->columns;
    const size_t src_stride = m->stride;
    const size_t dst_stride = m_out->stride;

    for (int i=0; i < src_rows; i++) {
        const float* src_row = src + ((size_t)i * src_stride);
        for (int j=0; j < src_cols; j++) {
            dst[dst_stride*(size_t)j+i] = src_row[j];
        }
    }

    return MAT_SUCCESS;
}

int madd(const matrix_s* a, const matrix_s* b, matrix_s* m_out) {
    if (a == NULL || b == NULL || m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (a->columns != b->columns ||
        a->rows != b->rows ||
        m_out->columns != a->columns ||
        m_out->rows != a->rows)
        return MAT_ERROR_DIMENSION_MISMATCH;

    const float* src_a = a->data;
    const float* src_b = b->data;
    float* dst = m_out->data;
    const int rows = a->rows;
    const int cols = a->columns;
    const size_t a_stride = a->stride;
    const size_t b_stride = b->stride;
    const size_t dst_stride = m_out->stride;

    for (int i=0; i < rows; i++) {
        const float* a_row = src_a +((size_t)i * a_stride);
        const float* b_row = src_b +((size_t)i * b_stride);
        float* dst_row = dst + ((size_t)i * dst_stride);
        int j = 0;
        for (; j <= cols-8 ; j+=8) {
            __m256 vec_a = _mm256_load_ps(&a_row[j]);
            __m256 vec_b = _mm256_load_ps(&b_row[j]);

            __m256 vec_res = _mm256_add_ps(vec_a, vec_b);
            _mm256_store_ps(&dst_row[j], vec_res);
        }

        for (; j < cols; j++) {
            dst_row[j] = a_row[j] + b_row[j];
        }
    }

    return MAT_SUCCESS;
}

int msub(const matrix_s* a, const matrix_s* b, matrix_s* m_out) {
    if (a == NULL || b == NULL || m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (a->columns != b->columns ||
        a->rows != b->rows ||
        m_out->columns != a->columns ||
        m_out->rows != a->rows)
        return MAT_ERROR_DIMENSION_MISMATCH;

    const float* src_a = a->data;
    const float* src_b = b->data;
    float* dst = m_out->data;
    const int rows = a->rows;
    const int cols = a->columns;
    const size_t a_stride = a->stride;
    const size_t b_stride = b->stride;
    const size_t dst_stride = m_out->stride;

    for (int i=0; i < rows; i++) {
        const float* a_row = src_a +((size_t)i * a_stride);
        const float* b_row = src_b +((size_t)i * b_stride);
        float* dst_row = dst + ((size_t)i * dst_stride);
        int j = 0;

        for (; j <= cols-8; j+=8) {
            __m256 vec_a = _mm256_load_ps(&a_row[j]);
            __m256 vec_b = _mm256_load_ps(&b_row[j]);

            __m256 vec_res = _mm256_sub_ps(vec_a, vec_b);
            _mm256_store_ps(&dst_row[j], vec_res);
        }
        for (; j < cols; j++) {
            dst_row[j] = a_row[j] - b_row[j];
        }
    }

    return MAT_SUCCESS;
}

int mmul_scalar(const matrix_s* m, float s, matrix_s* m_out) {
    if (m == NULL || m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (m == m_out) return MAT_ALIASING_NOT_ALLOWED;

    const float* restrict src = m->data;
    float* restrict dst = m_out->data;
    const int rows = m->rows;
    const int cols = m->columns;
    const size_t src_stride = m->stride;
    const size_t dst_stride = m_out->stride;
    __m256 vec_s = _mm256_set1_ps(s);

    for (int i=0; i < rows; i++) {
        const float* src_row = src + ((size_t)i * src_stride);
        float* dst_row = dst + ((size_t)i * dst_stride);
        int j = 0;
        for (; j <= cols-8; j+=8) {
            __m256 vec_src = _mm256_load_ps(&src_row[j]);
            __m256 vec_res = _mm256_mul_ps(vec_src, vec_s);
            _mm256_store_ps(&dst_row[j], vec_res);
        }
        for (; j < cols; j++) {
            dst_row[j] = src_row[j]*s;
        }
    }

    return MAT_SUCCESS;
}

MATRIX_INTERNAL int mmul_small(const matrix_s* a, const matrix_s* b, matrix_s* m_out) {
    const float* restrict src_a = a->data;
    const float* restrict src_b = b->data;
    float* restrict dst = m_out->data;
    const int src_a_rows = a->rows;
    const int src_a_cols = a->columns;
    const size_t src_a_stride = a->stride;
    const int src_b_cols = b->columns;
    const size_t src_b_stride = b->stride;
    const size_t dst_stride = m_out->stride;


    for(int i=0; i < src_a_rows; i++) {
        const size_t a_ofst = (size_t)i * src_a_stride;
        float* restrict dst_row = dst + (i * dst_stride);
        for(int j = 0; j < src_a_cols ; j++) {
            __m256 vec_a = _mm256_set1_ps(src_a[a_ofst+j]);
            const float* restrict b_row = src_b + (j * src_b_stride);
            int k = 0;
            for (; k <= src_b_cols-1; k+=8) {
                __m256 vec_dst = _mm256_load_ps(&dst_row[k]);
                __m256 vec_b = _mm256_load_ps(&b_row[k]);
                vec_dst = _mm256_fmadd_ps(vec_a, vec_b, vec_dst);
                _mm256_store_ps(&dst_row[k], vec_dst);
            }
            for (; k < src_b_cols; k++)
                dst_row[k] += src_a[a_ofst + j] * b_row[k];
        }
    }
    return MAT_SUCCESS;
}

MATRIX_INTERNAL int mmul_tpose(const matrix_s* a, const matrix_s* b, matrix_s* m_out) {
    // Transpose the matrix to improve cache locality
    matrix_s *b_t;
    matrix_status_s res = mnew(b->columns, b->rows, &b_t);

    if (res != MAT_SUCCESS) return MAT_ERROR_ALLOCATION_FAILED;
    res = mtpose(b, b_t);
    if (res != MAT_SUCCESS){
        mfree(&b_t);
        return res;
    }

    const float* restrict src_a = a->data;
    const float* restrict src_b = b_t->data;
    float* restrict dst = m_out->data;
    const int src_a_rows = a->rows;
    const int src_a_cols = a->columns;
    const size_t src_a_stride = a->stride;
    const int src_b_rows = b_t->rows;
    const size_t src_b_stride = b_t->stride;
    const size_t dst_stride = m_out->stride;

    for (int i=0; i < src_a_rows; i++) {
        const float* a_row = src_a + ((size_t)i * src_a_stride);
        for (int j=0; j < src_b_rows; j++) {
            const float* b_row = src_b + ((size_t)j * src_b_stride);
            __m256 vec_sum = _mm256_setzero_ps();
            int k = 0;
            // SIMD vectorized loop
            for (; k <= src_a_cols-8 ; k+=8) {
                __m256 vec_a = _mm256_load_ps(&a_row[k]);
                __m256 vec_b = _mm256_load_ps(&b_row[k]);
                vec_sum = _mm256_fmadd_ps(vec_a, vec_b, vec_sum);
            }
            // Folding the register to a single float
            __m128 vlow = _mm256_castps256_ps128(vec_sum);
            __m128 vhigh = _mm256_extractf128_ps(vec_sum, 1);
            vlow = _mm_add_ps(vlow, vhigh);
            __m128 shuf = _mm_movehl_ps(vlow, vlow);
            vlow = _mm_add_ps(vlow, shuf);
            shuf = _mm_shuffle_ps(vlow, vlow, _MM_SHUFFLE(2, 3, 0, 1));
            vlow = _mm_add_ps(vlow, shuf);
            float sum = _mm_cvtss_f32(vlow);
            // Cleanup loop
            for (; k < src_a_cols; k++) {
                sum += a_row[k] * b_row[k];
            }
            // Storing the cumulative result
            dst[i*dst_stride+j] = sum;
        }
    }
    mfree(&b_t);
    return MAT_SUCCESS;
}

MATRIX_INTERNAL int mmul_lt(const matrix_s* a, const matrix_s* b, matrix_s* m_out) {
    // Transpose the matrix to improve cache locality
     matrix_s *b_t;
     matrix_status_s res = mnew(b->columns, b->rows, &b_t);

     if (res != MAT_SUCCESS) return MAT_ERROR_ALLOCATION_FAILED;
     res = mtpose (b, b_t);
     if (res != MAT_SUCCESS) {
         mfree(&b_t);
         return res;
     };

     const float* restrict src_a = a->data;
     const int src_a_rows = a->rows;
     const int src_a_cols = a->columns;
     const size_t src_a_stride = a->stride;
     const float* restrict src_b = b_t->data;
     const int src_b_rows = b_t->rows;
     const size_t src_b_stride = b_t->stride;
     float* restrict dst = m_out->data;
     const int dst_rows = m_out->rows;
     const int dst_cols = m_out->columns;
     const size_t dst_stride = m_out->stride;

     // Begin tiled jumping
     for (size_t ii=0; ii < src_a_rows; ii += TILE_SIZE) {
         for (size_t jj=0; jj < src_b_rows; jj += TILE_SIZE) {
             for (size_t kk=0; kk < src_a_cols; kk += TILE_SIZE) {
                 // Set upper boundaries for the block to avoid reading into the padding
                 const size_t i_bound = ((ii + TILE_SIZE) > src_a_rows) ? src_a_rows : (ii + TILE_SIZE);
                 const size_t j_bound = ((jj + TILE_SIZE) > src_b_rows) ? src_b_rows : (jj + TILE_SIZE);
                 const size_t k_bound = ((kk + TILE_SIZE) > src_a_cols) ? src_a_cols : (kk + TILE_SIZE);

                 for (size_t i=ii; i < i_bound; i++) {
                     for (size_t j=jj; j < j_bound; j++) {
                         int k = kk;
                         __m256 vec_sum = _mm256_setzero_ps();
                         for (; k <= (int)k_bound-8; k+=8) {
                             __m256 vec_a = _mm256_load_ps(&src_a[i*src_a_stride+k]);
                             __m256 vec_b = _mm256_load_ps(&src_b[j*src_b_stride+k]);
                             vec_sum = _mm256_fmadd_ps(vec_a, vec_b, vec_sum);
                         }
                         // Folding the register to a single float
                        __m128 vlow = _mm256_castps256_ps128(vec_sum);
                        __m128 vhigh = _mm256_extractf128_ps(vec_sum, 1);
                        vlow = _mm_add_ps(vlow, vhigh);
                        __m128 shuf = _mm_movehl_ps(vlow, vlow);
                        vlow = _mm_add_ps(vlow, shuf);
                        shuf = _mm_shuffle_ps(vlow, vlow, _MM_SHUFFLE(2, 3, 0, 1));
                        vlow = _mm_add_ps(vlow, shuf);
                        float sum = _mm_cvtss_f32(vlow);
                        for (; k < k_bound; k++) {
                            sum += src_a[i*src_a_stride+k] * src_b[j*src_b_stride+k];
                        }
                        dst[i*dst_stride+j] += sum;
                    }
                }
            }
        }
    }
    mfree(&b_t);
    return MAT_SUCCESS;
}

int mmul(const matrix_s* a, const matrix_s* b, matrix_s* m_out) {
    if (a == NULL || b == NULL || m_out == NULL) return MAT_ERROR_NULL_POINTER;
    if (m_out == a || m_out == b) return MAT_ALIASING_NOT_ALLOWED;
    if (a->columns != b->rows ||
        m_out->rows != a ->rows ||
        m_out->columns != b-> columns)
        return MAT_ERROR_DIMENSION_MISMATCH;

    const int tpose_threshold = 32;
    const size_t working_mem = (((size_t)a->rows * a->stride) + ((size_t)b->rows * b->stride) + ((size_t)m_out->rows * m_out->stride)) * sizeof(float);

    if (working_mem > CACHE_THRESHOLD_BYTES) return mmul_lt(a, b, m_out);
    if (b->columns > tpose_threshold) return mmul_tpose(a, b, m_out);
    return mmul_small(a, b, m_out);
}

