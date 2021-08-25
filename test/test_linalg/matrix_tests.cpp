// ----------------------------------------------------------------------------
// MATRIX OBJECT UNIT TESTS
//
// Code By: Michael Wrona
// Created: 24 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests for matrix objects.
 */


#ifdef UNIT_TEST
#include "matrix_tests.h"

// Default matrix shape
void test_matrix_default_rows(void)
{
    Matrix mat;
    TEST_ASSERT_EQUAL((size_t)3, mat.rows);

}

void test_matrix_default_cols(void)
{
    Matrix mat;
    TEST_ASSERT_EQUAL((size_t)3, mat.cols);

}

void test_matrix_default_vals(void)
{
    size_t i;
    size_t defaultRows;
    size_t defaultCols;
    size_t rc;
    Matrix mat;
    float *testMat;
    float **pmat;
    float defaultVal;

    defaultRows = 3;
    defaultCols = 3;
    defaultVal = 0.0f;
    rc = defaultRows * defaultCols;

    testMat = new float[9];
    for (i = 1, *pmat = testMat; i < defaultRows; i++)
        *(pmat + i) = *(pmat + i - 1) + defaultCols;
    
    for (i = 0; i < rc; i++)
        testMat[i] = defaultVal;
    
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(testMat, mat.mat, 9);
}

#endif
