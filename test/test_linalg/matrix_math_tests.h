// ----------------------------------------------------------------------------
// MATRIX MATH & LINEAR ALGEBRA LIB UNIT TESTS
//
// Code By: Michael Wrona
// Created: 24 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests for the matrix math and linear algebra library.
 */

#ifdef UNIT_TEST
#pragma once

#include <unity.h>
#include <Arduino.h>
#include "maths/vectors.h"
#include "maths/matrices.h"
#include "maths/matrix_math.h"

void test_linalg_VectorfFill(void);
void test_linalg_VectorfAdd(void);
void test_linalg_VectorfAccumulate(void);
void test_linalg_VectorfSubtract(void);
void test_linalg_MatrixFill(void);
void test_linalg_MatrixAdd(void);
void test_linalg_MatrixSubtract(void);
void test_linalg_MatrixVectorfMult(void);
void test_linalg_MatrixMultiply(void);
void test_linalg_MatrixCholeskyDecomp(void);
void test_linalg_MatrixInverseCholesky(void);

float* allocate_vectorf(size_t len, size_t fillVal);
float* allocate_matrix(size_t rows, size_t cols, float fillVal);



#endif
