#ifndef MATRIX_H
#define MATRIX_H

/**
 * @brief A structure representing a 2D matrix stored as a flat array.
 */
typedef struct {
    int rows;
    int columns;
    int stride;
    float* data;
} matrix_s;

/**
 * @brief Enumeration of all handled status codes.
 */
typedef enum {
    MAT_SUCCESS = 0,
    MAT_ERROR_NULL_POINTER = 1,
    MAT_ERROR_NULL_DATA_POINTER = 2,
    MAT_ERROR_OUT_OF_BOUNDS = 3,
    MAT_ERROR_DIMENSION_MISMATCH = 4,
    MAT_ERROR_NOT_SQUARE = 5,
    MAT_ERROR_ALLOCATION_FAILED = 6,
    MAT_ALIASING_NOT_ALLOWED = 7,
} matrix_status_s;

/**
 *  @brief Allocate a new matrix initialized with zeroes.
 *  @param rows Number of rows.
 *  @param cols Number of columns.
 *  @param m Pointer to the new matrix.
 *  @return matrix_status_s Status code of the operation.
 */
int mnew(int rows, int cols, matrix_s** m_out);

/**
 * @brief De-allocate a matrix.
 * @param m Matrix to be freed.
 * @return matrix_status_s Status code of the operation.
 */
int mfree(matrix_s** m);

/**
 * @brief Print a matrix on stdout.
 * @param m Matrix to be printed.
 *  @return matrix_status_s Status code of the operation.
 */
int mprint(const matrix_s* m);

/**
 * @brief Determines if two matrices are equal.
 * @param a Matrix A.
 * @param b Matrix B.
 * @param res Pointer to a boolean value to store the comparison result.
 * @return matrix_status_s Status code of the operation.
 */
int meq(const matrix_s* a, const matrix_s* b, bool* res);

/**
 * @brief Write a single `float` value to a position in a matrix.
 * @param m Matrix to write to.
 * @param row Row number to write to.
 * @param col Column number to write to.
 * @param val `float` value to write in the matrix.
 * @return matrix_status_s Status code of the operation.
 */
int mwrite(matrix_s* m, int row, int col, float val);

/**
 * @brief Read a value from a matrix.
 * @param m Matrix to read from.
 * @param row Row number to read from.
 * @param col Column number to read from.
 * @param res Pointer to the variable to store the result in.
 * @return matrix_status_s Status code of the operation.
 */
int mread(const matrix_s* m, int row, int col, float* res);

/**
 * @brief Transposes a matrix.
 * @param m Matrix to be transposed.
 * @param m_out Pointer to the transposed matrix.
 * @return matrix_status_s Status code of the operation.
 */
int mtpose(const matrix_s* m, matrix_s* m_out);

/**
 * @brief Add two matrices together in a new matrix.
 * @param a First matrix to be addedd.
 * @param b Second matrix to be addedd.
 * @param m_out Matrix pointer to the resulting matrix.
 * @return matrix_status_s Status code of the operation.
 */
int madd(const matrix_s* a, const matrix_s* b, matrix_s* m_out);

/**
 * @brief Subtract matrix b from matrix a.
 * @param a Matrix to subtract from.
 * @param b Matrix to subtract.
 * @param m_out Matrix pointer to the resulting matrix.
 * @return matrix_status_s Status code of the operation.
 */
int msub(const matrix_s* a, const matrix_s* b, matrix_s* m_out);

/**
 * @brief Multiplicate a matrix by a scalar value.
 * @param m Matrix to be multiplied.
 * @param s Scalar value.
 * @param m_out Matrix pointer to the resulting matrix.
 * @return matrix_status_s Status code of the operation.
 */
int mmul_scalar(const matrix_s* m, float s, matrix_s* m_out);

/**
 * @brief Multiplicate two matrices.
 * @param a First matrix to be multiplied.
 * @param b Second matrix to be multiplied.
 * @param m_out Matrix pointer to the resulting matrix.
 * @return matrix_status_s Status code of the operation.
 */
int mmul(const matrix_s* a, const matrix_s* b, matrix_s* m_out);

#endif
