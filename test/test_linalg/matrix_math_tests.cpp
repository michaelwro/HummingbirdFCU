// ----------------------------------------------------------------------------
// MATRIX MATH & LINEAR ALGEBRA LIB UNIT TESTS
//
// Code By: Michael Wrona
// Created: 24 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests for the matrix math and linear algebra library.
 * unit_test(expected, acutal)
 */


#ifdef UNIT_TEST
#include "matrix_math_tests.h"


/* Test vector fill */
void test_linalg_VectorfFill(void)
{
    size_t len = 5;
    float fillVal = 25.0f;
    Vectorf vec(len);
    
    // expected
    float *b = allocate_vectorf(len, fillVal);
    
    // actual
    VectorfFill(vec.vec, fillVal, len);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(b, vec.vec, len);
}


/* Test vector-vector addition */
void test_linalg_VectorfAdd(void)
{
    size_t i;
    size_t len = 5;
    
    Vectorf a(len);
    Vectorf b(len);
    Vectorf c(len);
    Vectorf res(len);

    for (i = 0; i < len; i++)
    {
        a.vec[i] = (float)i;
        b.vec[i] = (float)i;
        res.vec[i] = (float)(i + i);  // expected
    }

    VectorfAdd(c.vec, a.vec, b.vec, len);  // actual

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(res.vec, c.vec, len);
}


/* Test vector-vector accumulate */
void test_linalg_VectorfAccumulate(void)
{
    size_t i;
    size_t len = 5;
    
    Vectorf a(len);
    Vectorf b(len);
    Vectorf res(len);

    for (i = 0; i < len; i++)
    {
        a.vec[i] = (float)i;
        b.vec[i] = (float)i;
        res.vec[i] = (float)(i + i);  // expected
    }

    VectorfAccumulate(a.vec, b.vec, len);  // actual

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(res.vec, a.vec, len);
}

/* Test vector-vector subtraction */
void test_linalg_VectorfSubtract(void)
{
    size_t i;
    size_t len = 5;
    
    Vectorf a(len);
    Vectorf b(len);
    Vectorf c(len);
    Vectorf res(len);

    for (i = 0; i < len; i++)
    {
        a.vec[i] = (float)i + 4.0f;
        b.vec[i] = (float)i - 1.0f;
        res.vec[i] = (float)(5.0f);  // expected
    }

    VectorfSubtract(c.vec, a.vec, b.vec, len);  // actual

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(res.vec, c.vec, len);
}


/* Test matrix fill */
void test_linalg_MatrixFill(void)
{
    size_t i, j;
    size_t r = 4;
    size_t c = 4;
    float fill = 12.0f;

    Matrix A(r, c);
    float *Expected = allocate_matrix(r, c, fill);

    // expected
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            MATRIX_ELEMENT(Expected, i, j, r, c) = fill;
    
    MatrixFill(fill, A.mat, r, c);  // actual

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(Expected, A.mat, r*c);
}

/* Test matrix-matrix addition */
void test_linalg_MatrixAdd(void)
{
    size_t i, j;
    size_t r = 4;
    size_t c = 4;
    size_t rc = r * c;

    Matrix A(r, c);
    Matrix B(r, c);
    Matrix C(r, c);

    // expected result
    Matrix Expected(r, c);
    float result[rc] = {6, 6, 6, 6, 8, 8, 8, 8, 10, 10, 10, 10, 12, 12, 12, 12};
    for (i = 0; i < rc; i++)
        Expected.mat[i] = result[i];

    // make A
    // cast i and j to float becauze size_t cannot be less than zero!
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            MATRIX_ELEMENT(A.mat, i, j, r, c) = (float)i + (float)j + 1.0f;
    
    // make B
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            MATRIX_ELEMENT(B.mat, i, j, r, c) = (float)i - (float)j + 5.0f;
    
    // perform C = A + B
    MatrixAdd(C.mat, A.mat, B.mat, r, c);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(Expected.mat, C.mat, rc);
}


/* Test matrix-matrix subtraction */
void test_linalg_MatrixSubtract(void)
{
    size_t i, j;
    size_t r = 4;
    size_t c = 4;
    size_t rc = r * c;

    Matrix A(r, c);
    Matrix B(r, c);
    Matrix C(r, c);

    // expected result
    Matrix Expected(r, c);
    float result[rc] = {-4, -2,  0,  2, -4, -2,  0,  2, -4, -2,  0,  2, -4, -2,  0,  2};
    for (i = 0; i < rc; i++)
        Expected.mat[i] = result[i];

    // make A
    // cast i and j to float becauze size_t cannot be less than zero!
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            MATRIX_ELEMENT(A.mat, i, j, r, c) = (float)i + (float)j + 1.0f;
    
    // make B
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            MATRIX_ELEMENT(B.mat, i, j, r, c) = (float)i - (float)j + 5.0f;
    
    // perform C = A + B
    MatrixSubtract(C.mat, A.mat, B.mat, r, c);  // actual

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(Expected.mat, C.mat, rc);
}


/* Test matrix-vector multiplication */
void test_linalg_MatrixVectorfMult(void)
{
    size_t i, j;
    size_t r = 4;
    size_t c = 4;

    // b = Ax
    Matrix A(r, c);
    Vectorf x(r);
    Vectorf b(r);

    // expected result
    Vectorf Expected(r);
    float result[r] = {70, 98, 126, 154};
    for (i = 0; i < r; i++)
        Expected.vec[i] = result[i];

    // make A
    // cast i and j to float becauze size_t cannot be less than zero!
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            MATRIX_ELEMENT(A.mat, i, j, r, c) = (float)i + (float)j + 1.0f;
    
    // make x
    for (i = 0; i < r; i++)
        x.vec[i] = 7.0f;
    
    // perform b = A*x
    MatrixVectorfMult(b.vec, A.mat, x.vec, r, c);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(Expected.vec, b.vec, r);
}


/* Test matrix-matrix multiplication */
void test_linalg_MatrixMultiply(void)
{
    size_t i, j;
    size_t r = 4;
    size_t c = 4;
    size_t rc = r * c;

    Matrix A(r, c);
    Matrix B(r, c);
    Matrix C(r, c);

    // expected result
    Matrix Expected(r, c);
    float result[rc] = {70, 60, 50, 40, 96, 82, 68, 54, 122, 104, 86, 68, 148, 126, 104, 82};
    for (i = 0; i < rc; i++)
        Expected.mat[i] = result[i];

    // make A
    // cast i and j to float becauze size_t cannot be less than zero!
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            MATRIX_ELEMENT(A.mat, i, j, r, c) = (float)i + (float)j + 1.0f;
    
    // make B
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            MATRIX_ELEMENT(B.mat, i, j, r, c) = (float)i - (float)j + 5.0f;
    
    // perform C = A + B
    MatrixMultiply(C.mat, A.mat, B.mat, r, c, r, c);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(Expected.mat, C.mat, rc);   
}


/* Test Cholesky decomposition of a positive definite matrix */
void test_linalg_MatrixCholeskyDecomp(void)
{
    size_t i, j;
    size_t dim = 3;

    // input matrix
    Matrix A(dim, dim);
    float orig[dim*dim] = {2, -1, 0, -1, 2, -1, 0, -1, 2};
    for (i = 0; i < dim*dim; i++)
        A.mat[i] = orig[i];

    // expected result
    Matrix A_chol(dim, dim);
    float result[dim*dim] = {1.41421356, -0.70710678, 0, -0.70710678, 1.22474487, -0.81649658, 0, -0.81649658, 1.15470054};
    for (i = 0; i < dim*dim; i++)
        A_chol.mat[i] = result[i];
    
    // perform chol(A) in-place
    _MatrixCholeskyDecomp(A.mat, dim);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(A_chol.mat, A.mat, dim * dim);
}


/* Test matrix inversion of a positive definite matrix via Cholesky decomposition */
void test_linalg_MatrixInverseCholesky(void)
{
    size_t i;
    size_t dim = 3;

    // input matrix
    Matrix A(dim, dim);
    float orig[dim*dim] = {2, -1, 0, -1, 2, -1, 0, -1, 2};
    for (i = 0; i < dim*dim; i++)
        A.mat[i] = orig[i];

    // expected result
    Matrix A_inv(dim, dim);
    float result[dim*dim] = {0.75, 0.5, 0.25, 0.5, 1, 0.5, 0.25, 0.5, 0.75};
    for (i = 0; i < dim*dim; i++)
        A_inv.mat[i] = result[i];
    
    // perform inv(A) in-place
    MatrixInverseCholesky(A.mat, dim);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(A_inv.mat, A.mat, dim * dim);
}







/* Allocate a vector and fill it */
float* allocate_vectorf(size_t len, size_t fillVal)
{
    float *b = new float[len];
    for (size_t i = 0; i < len; i++)
        b[i] = fillVal;
    return b;
}

/* Allocate a matrix and fill it */
float* allocate_matrix(size_t rows, size_t cols, float fillVal)
{
    float *mat;
    float **pmat;
    size_t i; 
    size_t rc = rows * cols;

    mat = new float[rc];
    pmat = new float*[rows];
    for (i = 1, *pmat = mat; i < rows; i++)
        *(pmat + i) = *(pmat + i - 1) + cols;
    
    for (i = 0; i < rc; i++)
        mat[i] = fillVal;
    
    return mat;
}

#endif
