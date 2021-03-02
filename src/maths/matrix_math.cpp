/**
 * USES SINGLE-PRECISION FLOATS!
 * 
 * The library assumes the 2D array is one long dynamic array in memory (row 
 * major). The array indexing may be a bit strange, but since we allocated one 
 * long array in memory, it sould help a bit with cache performance. See
 * below for an example of this.
 * 
 * I give the following site credit for the matrix math algorithms. It was a 
 * significant inspiration and resource for algorithms! 
 * http://www.mymathlib.com/matrices/
 * 
 *
 * Vector operations include:
 * - Fill
 * - Addition
 * - Accumulate (in-place)
 * - Subtract
 * 
 * Matrix operations include:
 * - Fill
 * - Transpose
 * - Square matrix transpose (in-place)
 * - Addition
 * - Add identity (I + A)
 * - Subtract identity (I - A)
 * - Accumulate (in-place)
 * - Subtraction
 * - Subtract accumulate (in-place)
 * - Negate (-1 * A)
 * - Matrix-vector multiply
 * - Matrix-matrix multiply
 * - Inversion via Cholesky decomposition
 * - Special multiplication (A * B^T)
 * 
 * -----------------------------------------------
 * How to Allocate and Access a 1D Array (vector):
 * 
 *   float *b = new float[rows];  // Allocate 1D aray/vector
 * 
 *   for (size_t i = 0; i < rows; i++)
 *     b[i] = 0.0f;  // Or, use: *(b + i)
 * --------------------------------------
 * How to Allocate and Access a 2D Array:
 * 
 *   float *A = new float[rows * cols];  // Allocate 2D array
 *   
 *   for (size_t i = 0; i < rows; i++)  // Set value
 *     for (size_t j = 0; j < cols; j++)
 *       *(A + i*cols + j) = 0.0f;  // Row-major
 * ------------------------------------
 * 
 * Resources
 * ---------
 * https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/
 * https://eli.thegreenplace.net/2015/memory-layout-of-multi-dimensional-arrays
 */

#include <Arduino.h>
#include "maths/matrix_math.h"


// ----------------------------------------------------------------------------
// VectorFill(float *vec, float fill, size_t n)
// ----------------------------------------------------------------------------
/**
 * Fill a vector with a specified value. Use this to initialize arrays before 
 * they're used.
 * 
 * @param fill  Value to fill array with
 * @param a     Vector of interest
 * @param n     Size/length of vector
 */
void VectorFill(float *vec, float fill, size_t n)
{
    size_t i;
    
    for (i = 0; i < n; i++)
        vec[i] = fill;
}


// ----------------------------------------------------------------------------
// VectorAdd(float *c, float *a, float *b, size_t n)
// ----------------------------------------------------------------------------
/**
 * Add two vectors together and output result to a new vector. Assumes vectors
 * have similar dimensions. c <- a + b
 * 
 * @param a     Pointer to first vector
 * @param b     Pointer to second vector
 * @param c     Pointer to output vector
 * @param n     Length of vector
 */
void VectorAdd(float *c, float *a, float *b, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        c[i] = a[i] + b[i];
}


// ----------------------------------------------------------------------------
// VectorAccumulate(float *a, float *b, size_t n)
// ----------------------------------------------------------------------------
/**
 * a <- a + b. Add two vectors in-place and output result to vector a. Assumes 
 * vectors have similar dimensions.
 * 
 * @param a     Pointer to first vector
 * @param b     Pointer to second vector
 * @param n     Length of vector
 */
void VectorAccumulate(float *a, float *b, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        a[i] += b[i];
}


// ----------------------------------------------------------------------------
// VectorSubtract(float *c, float *a, float *b, size_t n)
// ----------------------------------------------------------------------------
/**
 * Subtract two vectors and output result to a new vector. Assumes vectors 
 * have similar dimensions. c <- a - b
 * 
 * @param a     Pointer to first vector
 * @param b     Pointer to second vector
 * @param c     Pointer to output vector
 * @param n     Length of vector
 */
void VectorSubtract(float *c, float *a, float *b, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        c[i] = a[i] - b[i];
}


// ----------------------------------------------------------------------------
// MatrixFill(float fill, float *A, size_t rows, size_t cols)
// ----------------------------------------------------------------------------
/**
 * Fill a matrix with a specified value. Use this to initialize arrays 
 * before they're used.
 * 
 * @param fill  Value to fill array with
 * @param A     Matrix of interest
 * @param r     Rows
 * @param c     Columns
 */
void MatrixFill(float fill, float *A, size_t rows, size_t cols)
{
    size_t i, j;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            *A++ = fill;
        }
    }
}


// ----------------------------------------------------------------------------
// MatrixTranspose(float *A, float *At, size_t arows, size_t acols)
// ----------------------------------------------------------------------------
/**
 * Compute the transpose of a matrix and output it to another one. Matrix "A"
 * has dimensions (r, c) and the transpose "At" has dimensions (c, r). See the 
 * following link for original source code: 
 * http://www.mymathlib.com/c_source/matrices/datamovement/transpose_matrix.c
 * 
 * @param A     Original matrix
 * @param At    Transpose of matrix
 * @param arows Matrix rows in the orig.
 * @param acols Matrix columns in the orig.
 */
void MatrixTranspose(float *A, float *At, size_t arows, size_t acols)
{
    size_t i, j;
    float *pA;
    float *pAt;

    for (i = 0; i < arows; At += 1, A += acols, i++)
    {
        pAt = At;
        pA = A;
        for (j = 0; j < acols; pAt += arows, j++)
            *pAt = pA[j];
    }
}


// ----------------------------------------------------------------------------
// MatrixTransposeSquare(float *A, float *At, size_t n)
// ----------------------------------------------------------------------------
/**
 * Compute the transpose of a SQUARE (n, n) matrix in-place. See the following 
 * link for original source code: 
 * http://www.mymathlib.com/c_source/matrices/datamovement/transpose_square_matrix.c
 * 
 * @param A     Original matrix
 * @param n     Rows/columns of the square matrix
 */
void MatrixTransposeSquare(float *A, size_t n)
{
    size_t i, j;
    float *pA, *pAt;
    float temp;

    for (i = 0; i < n; A += n+1, i++)
    {
        pA = A + 1;
        pAt = A + n;
        for (j = i+1; j < n; pA++, pAt += n, j++)
        {
            temp = *pAt;
            *pAt = *pA;
            *pA = temp;
        }
    }
}


// ----------------------------------------------------------------------------
// MatrixAdd(float *A, float *B, float *C, size_t rows, size_t cols)
// ----------------------------------------------------------------------------
/**
 * C <- A + B. Add matrix A to matrix B and output result to a new array C. 
 * MAKE SURE THE DIMENSIONS OF THE ARRAYS MATCH!
 * 
 * @param C     Output matrix
 * @param A     Matrix A
 * @param B     Matrix B
 * @param rows  Matrix rows
 * @param cols  Matrix columns
 */ 
void MatrixAdd(float *C, float *A, float *B, size_t rows, size_t cols)
{
    size_t i;
    size_t n = rows * cols;

    for (i = 0; i < n; i++)
        C[i] = A[i] + B[i];
}


// ----------------------------------------------------------------------------
// MatrixAddIdentity(float *A, size_t rows, size_t cols)
// ----------------------------------------------------------------------------
/**
 * A <- I + A. Add identity matrix to matrix A in-place.
 * 
 * @param A     Matrix
 * @param rows  Matrix rows
 * @param cols  Matrix columns
 */
void MatrixAddIdentity(float *A, size_t rows, size_t cols)
{
    size_t i, n;

    n = (rows < cols) ? rows : cols;

    for (i = 0; i < n; A += (cols+1), i++)
        *A += 1.0f;
}


// ----------------------------------------------------------------------------
// MatrixSubtractIdentity(float *A, size_t rows, size_t cols)
// ----------------------------------------------------------------------------
/**
 * A <- I - A. Subtract matrix A from an identity matrix in-place.
 * 
 * @param A     Matrix
 * @param rows  Matrix rows
 * @param cols  Matrix columns
 */
void MatrixSubtractIdentity(float *A, size_t rows, size_t cols)
{
    size_t i, n;

    n = (rows < cols) ? rows : cols;

    for (i = 0; i < n; A += (cols+1), i++)
        *A = 1.0f - *A;
}


// ----------------------------------------------------------------------------
// MatrixAccumulate(float *A, float *B, size_t rows, size_t cols)
// ----------------------------------------------------------------------------
/**
 * A <- A + B. Add matrix B to matrix A in-place. A and B must have identical 
 * dimensions!
 * 
 * @param A     Matrix A
 * @param B     Matrix B
 * @param rows  Matrix rows
 * @param cols  Matrix columns
 */
void MatrixAccumulate(float *A, float *B, size_t rows, size_t cols)
{
    size_t i;
    size_t n = rows * cols;

    for (i = 0; i < n; i++)
        A[i] += B[i];
}


// ----------------------------------------------------------------------------
// MatrixSubtract(float *C, float *A, float *B, size_t rows, size_t cols)
// ----------------------------------------------------------------------------
/**
 * C <- A - B. Subtract matrix B from matrix A and output result to a new 
 * array C. MAKE SURE THE DIMENSIONS OF THE ARRAYS MATCH!
 * 
 * @param C     Output matrix
 * @param A     Matrix A
 * @param B     Matrix B
 * @param rows  Matrix rows
 * @param cols  Matrix columns
 */ 
void MatrixSubtract(float *C, float *A, float *B, size_t rows, size_t cols)
{
    size_t i;
    size_t n = rows * cols;

    for (i = 0; i < n; i++)
        C[i] = A[i] - B[i];
}


// ----------------------------------------------------------------------------
// MatrixSubAccumulate(float *A, float *B, size_t rows, size_t cols)
// ----------------------------------------------------------------------------
/**
 * A <- A - B. Subtract matrix B from matrix A in-place. A and B must have 
 * identical dimensions!
 * 
 * @param A     Matrix A
 * @param B     Matrix B
 * @param rows  Matrix rows
 * @param cols  Matrix columns
 */
void MatrixSubAccumulate(float *A, float *B, size_t rows, size_t cols)
{
    size_t i;
    size_t n = rows * cols;

    for (i = 0; i < n; i++)
        A[i] -= B[i];
}


// ----------------------------------------------------------------------------
// MatrixNegate(float *A, size_t rows, size_t cols)
// ----------------------------------------------------------------------------
/**
 * Multiply all elements in a matrix by -1.
 * 
 * @param A     Matrix
 * @param rows  Matrix rows
 * @param cols  Matrix columns
 */
void MatrixNegate(float *A, size_t rows, size_t cols)
{
    size_t i, j;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            *A++ *= -1.0f;
        }
    }
}


// ----------------------------------------------------------------------------
// MatrixVectorMult(float *outVec, float *A, float *b, 
//                  size_t matRows, size_t matCols)
// ----------------------------------------------------------------------------
/**
 * c <- A * b. Perform matrix-vector multiplication and output result to a 
 * new array. Make sure that the matrix columns match the vector rows!
 * 
 * @param outVec    Vector to output result to
 * @param A         Matrix
 * @param b         Vector
 * @param rows      Matrix rows
 * @param cols      Matrix columns
 */
void MatrixVectorMult(float *outVec, float *A, float *b, 
                    size_t rows, size_t cols)
{
    size_t i, j;

    for (i = 0; i < rows; A += cols, i++)
    {
        for (outVec[i] = 0.0f, j = 0; j < cols; j++)
            outVec[i] += A[j] * b[j];
    }
}


// ----------------------------------------------------------------------------
// MatrixMultiply(float *C, float *A, float *B, 
//                     size_t aRows, size_t aCols, size_t bRows, size_t bCols)
// ----------------------------------------------------------------------------
/**
 * C <- A * B. Perform matrix multiplication for two matrices. Outputs result to 
 * a new array C. MAKE SURE A COLUMNS EQUAL B ROWS. OUTPUT DIMENSION SHOULD BE 
 * (A_ROWS, B_COLS).
 * 
 * @param A     First matrix
 * @param B     Second matrix
 * @param C     Output array
 * @param aRows Rows of A
 * @param aCols Columns of A
 * @param bRows Rows of B
 * @param bCols Columns of B
 */
void MatrixMultiply(float *C, float *A, float *B, 
                    size_t aRows, size_t aCols, size_t bRows, size_t bCols)
{
    float *pB;
    float *p_B;
    size_t i, j, k;

    for (i = 0; i < aRows; A += aCols, i++)
    {
        for (p_B = B, j = 0; j < bCols; C++, p_B++, j++)  // lol, C++
        {
            pB = p_B;
            *C = 0.0f;
            for (k = 0; k < aCols; pB += bCols, k++)
                *C += *(A + k) * *pB;
        }
    }
}


// ----------------------------------------------------------------------------
// MatrixMultiply_ABt(float *C, float *A, float *B, 
//                      size_t arows, size_t acols, size_t brows)
// ----------------------------------------------------------------------------                    
/**
 * C <- A * B^T. Multiply matrix A by the transpose of matrix B, i.e. 
 * C = A*B'. A columns must match B rows! Outputs result to matrix C.
 * 
 * @param C     Output matrix (Arows, Brows)
 * @param A     Matrix A (Arows, Acols)
 * @param B     Matrix B (Brows, Bcols)
 * @param arows     Matrix A rows
 * @param acols     Matrix A columns
 * @param brows     Matrix B rows
 */
void MatrixMultiply_ABt(float *C, float *A, float *B, size_t arows, size_t acols, size_t brows)
{
    size_t i, j, k;
    float *pa;
    float *pb;

    for (i = 0; i < arows; A += acols, i++)
    {
        for (pb = B, j = 0; j < brows; C++, j++)  // lol
        {
            for (pa = A, *C = 0.0f, k = 0; k < acols; k++)
            {
                *C += *pa++ * *pb++;
            }
        }
    }
}


// ----------------------------------------------------------------------------
// MatrixInverseCholesky(float *A, size_t n)
// ----------------------------------------------------------------------------
/**
 * Compute the inverse of matrix A via Cholesky decomposition. This algorithm 
 * outputs replaces the input matrix with it's inverse. Note that Cholesky 
 * decomposition only works for square, symmetric, positive definite 
 * matrices. The original algorithm cab be found at: 
 * http://www.mymathlib.com/c_source/matrices/linearsystems/choleski.c
 * 
 * @param A     Input matrix, SPD
 * @param n     Rows /columns of the square matrix
 * @returns     True if successful, false if not.
 */
bool MatrixInverseCholesky(float *A, size_t n)
{
    size_t i, j, k;
    float *pi, *pj, *pk;
    float sum;
    
    // Begin by performing Cholesky decomposition on the input matrix.
    // Note that this will overwrite the original matrix!
    if (_MatrixCholeskyDecomp(A, n) == false)
    {
        #ifdef MATRIX_MATH_DEBUG
            DebugPort.println("MATRIX_MATH:MatrixInverseCholesky WARNING: Matrix is not SPD.");
        #endif
        // return false;  // TODO: raise error state?
    }

    // Invert the lower triangular matrix
    if (_MatrixLowerTriangularInverse(A, n) == false)
    {
        #ifdef MATRIX_MATH_DEBUG
            DebugPort.println("MATRIX_MATH:MatrixInverseCholesky WARNING: Zero element on the diagonal.");
        #endif
        // return false;  // TOD: raise error state?
    }

    // Perform multiplication
    for (i = 0, pi = A; i < n; i++, pi += n)
    {
        for (j = 0, pj = A; j <= i; j++, pj += n)
        {
            sum = 0.0f;
            for (k = i, pk = pi; k < n; k++, pk += n)
                sum += *(pk + i) * *(pk + j);
            
            *(pi + j) = sum;
            *(pj + i) = sum;
        }
    }

    return true;
}


/**
 * Perform Cholesky decomposition on a square, symmetric, positive 
 * definite matrix. Used in the MatrixInverseCholesky() routine. 
 * Original code can be found at: 
 * http://www.mymathlib.com/matrices/
 */
bool _MatrixCholeskyDecomp(float *A, size_t n)
{
    size_t i, k, p;
    float *p_Lk0;                   // pointer to L[k][0]
    float *p_Lkp;                   // pointer to L[k][p]  
    float *p_Lkk;                   // pointer to diagonal element on row k.
    float *p_Li0;                   // pointer to L[i][0]
    float reciprocal;

    for (k = 0, p_Lk0 = A; k < n; p_Lk0 += n, k++)
    {
        // Update pointer to row k diagonal element
        p_Lkk = p_Lk0 + k;

        // Calculate the difference of the diagonal element in row k
        // from the sum of squares of elements row k from column 0 to 
        // column k-1.

        for (p = 0, p_Lkp = p_Lk0; p < k; p_Lkp += 1, p++)
            *p_Lkk -= *p_Lkp * *p_Lkp;
        
        // If diagonal element is not positive, return the error code,
        // the matrix is not positive definite symmetric.
        if (*p_Lkk <= FLOAT_PREC_ZERO)
        {
            #ifdef MATRIX_MATH_DEBUG
                DebugPort.println("MATRIX_MATH:MatrixCholeskyDecomp WARNING: Matrix is not SPD.");
            #endif
            return false;
        }
        
        // Otherwise, take the square root of the diagonal element
        *p_Lkk = sqrtf_safe(*p_Lkk) ;
        reciprocal = 1.0f / *p_Lkk;

        // For rows i = k+1 to n-1, column k, calculate the difference
        // between the i,k th element and the inner product of the first
        // k-1 columns of row i and row k, then divide the difference by
        // the diagonal element in row k.
        // Store the transposed element in the upper triangular matrix.

        p_Li0 = p_Lk0 + n;
        for (i = k + 1; i < n; p_Li0 += n, i++)
        {
            for (p = 0; p < k; p++)
                *(p_Li0 + k) -= *(p_Li0 + p) * *(p_Lk0 + p);
            
            *(p_Li0 + k) *= reciprocal;
            *(p_Lk0 + i) = *(p_Li0 + k);
        }
    }

    return true;
}


/**
 * Compute the inverse of a lower triangular matrix. Used in 
 * the MatrixInverseCholesky() function. Original code can be found at: 
 * http://www.mymathlib.com/matrices/
 */
bool _MatrixLowerTriangularInverse(float *A, size_t n)
{
    size_t i, j, k;
    float *pi, *pj, *pk;
    float sum;

    // Invert the diagonal elements of the lower triangular matrix.
    for (k = 0, pk = A; k < n; pk += (n + 1), k++){
        if (fabsf(*pk) <= FLOAT_PREC_ZERO)
        {
            #ifdef MATRIX_MATH_DEBUG
                DebugPort.println("MATRIX_MATH:_MatrixLowerTriangularInverse WARNING: Zero element on the diagonal.");
            #endif
            return false;
        }
        else
        {
            *pk = 1.0f / *pk;
        }
    }

    // Invert the remaining lower triangular matrix
    for (i = 1, pi = A + n; i < n; i++, pi += n)
    {
        for (j = 0, pj = A; j < i; pj += n, j++)
        {
            sum = 0.0f;
            for (k = j, pk = pj; k < i; k++, pk += n)
                sum += *(pi + k) * *(pk + j);
            
            *(pi + j) = -sum * *(pi + i);
        }
    }

    return true;   
}




// bool MatrixIsPosDef(float *A, size_t rows, size_t cols)
// {
//     bool result = true;
//     size_t i, j, k;
//     float sum = 0.0f;  // Temp. variable
    
//     #ifdef MATRIX_MATH_DEBUG
//         if (rows != cols)
//         {
//             DebugPort.println("MATRIX_MATH:MatrixIsPosDef WARNING: Matrix must be square. Check that rows = columns.");
//         }
//     #endif

//     for (i = 0; i < rows; i++)
//     {
//         for (j = i; j < rows; j++)
//         {
//             sum = MATRIX_ELEMENT(A, i, j, rows, cols);
//             for (k = i-1; k >= 0; k--)
//                 sum -= MATRIX_ELEMENT(A, i, k, rows, cols) * MATRIX_ELEMENT(A, j, k, rows, cols);
            
//             if (i == j)
//             {
//                 if (sum <= 0.0f)
//                 {
//                     result = false;
//                 }
//             }
//         }
//     }

//     return result;
// }



// #ifdef MATRIX_MATH_DEBUG  // ==================================================
// // ----------------------------------------------------------------------------
// // PrintVector(float *a, size_t n)
// // ----------------------------------------------------------------------------
// /**
//  * Print a vector to the debug port.
//  * 
//  * @param a     Vector to print
//  * @param n     Length/size of vector
//  */
// void PrintVector(float *a, size_t n)
// {
//     size_t i;
//     DebugPort.print("[");
//     for (i = 0; i < n; i++)
//     {
//         DebugPort.print(VECTOR_ELEMENT(a, i), 4);
//         DebugPort.print((i != n-1) ? ", " : " ");
//     }
//     DebugPort.println("]");
// }


// // ----------------------------------------------------------------------------
// // PrintMatrix(float *A, size_t r, size_t c)
// // ----------------------------------------------------------------------------
// /**
//  * Print a matrix to the debug port.
//  * 
//  * @param A     Matrix to print
//  * @param r     Rows
//  * @param c     Columns
//  */
// void PrintMatrix(float *A, size_t r, size_t c)
// {
//     size_t i, j;

//     for (i = 0; i < r; i++)
//     {
//         DebugPort.print("[[ ");
//         for (j = 0; j < c; j++)
//         {
//             DebugPort.print(MATRIX_ELEMENT(A, i, j, r, c), 4);
//             DebugPort.print((j != c-1) ? ", " : "");
//         }
//         DebugPort.println(" ]]");
//     }
//     DebugPort.println("");
// }
// #endif  // ====================================================================
