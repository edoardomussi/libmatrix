#ifndef MATRIX_H
#define MATRIX_H

/**
 * \brief A structure representing a 2D matrix stored as a flat array.
 */
typedef struct {
    int rows;
    int columns;
    float* data;
} matrix_t;

/**
 *  \brief Allocate a new matrix initialized with zeroes.
 *  \param r Number of rows.
 *  \param c Number of columns.
 *  \return Pointer to the newly allocated matrix.
 */
matrix_t* mnew(int rows, int cols);

/**
 * \brief De-allocate a matrix.
 * \param m Matrix to be freed.
 * \return Your memory, hopefully.
 */
void mfree(matrix_t* m);

/**
 * \brief Print a matrix on stdout.
 * \param m Matrix to be printed.
 */
void mprint(matrix_t* m);

/**
 * \brief Write a single `float` value to a position in a matrix.
 * \param m Matrix to write to.
 * \param row Row number to write to.
 * \param col Column number to write to.
 * \param val `float` value to write in the matrix.
 */
int mwrite(matrix_t* m, int row, int col, float val);

/**
 * \brief Read a value from a matrix.
 * \param m Matrix to read from.
 * \param row Row number to read from.
 * \param col Column number to read from.
 */
float mread(matrix_t* m, int row, int col);

/**
 * \brief Add two matrices together in a new matrix.
 * \param a First matrix to be addedd.
 * \param b Second matrix to be addedd.
 * \return matrix_t* Pointer to the resulting matrix.
 */
matrix_t* madd(matrix_t* a, matrix_t* b);

/**
 * \brief Multiplicate a matrix by a scalar value.
 * \param m Matrix to be multiplied.
 * \param s Scalar value.
 * \return Nothing, the original matrix is overwritten.
 */
void mmul_scalar(matrix_t* m, float s);

/**
 * \brief Multiplicate two matrices.
 * \param a First matrix to be multiplied.
 * \param b Second matrix to be multiplied.
 * \return matrix_t* Pointer to the resulting matrix.
 */
matrix_t* mmul(matrix_t* a, matrix_t* b);

#endif
