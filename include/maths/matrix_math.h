// ----------------------------------------------------------------------------
// MATRIX MATH LIBRARY FOR TEENSY 4.1
// 
// Code By: Michael Wrona
// Created: 14 Feb 2021
// ----------------------------------------------------------------------------
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

#pragma once

#include <Arduino.h>
#include <math.h>
#include "hummingbird_config.h"
#include "maths/math_functs.h"
#include "maths/matrices.h"
#include "maths/vectors.h"


#if defined(DEBUG) && defined(DebugPort)
    /* Enable/disable printing debug messages to DebugPort */
    #define MATRIX_MATH_DEBUG
#endif


/* VECTOR FUNCTIONS */
void VectorFill(float *vec, float fill, size_t n);
void VectorAdd(float *c, float *a, float *b, size_t n);
void VectorAccumulate(float *a, float *b, size_t n);
void VectorSubtract(float *c, float *a, float *b, size_t n);

/* MATRIX FUNCTIONS */
void MatrixFill(float fill, float *A, size_t rows, size_t cols);
void MatrixTranspose(float *A, float *At, size_t arows, size_t acols);
void MatrixTransposeSquare(float *A, size_t n);

// Addition
void MatrixAdd(float *C, float *A, float *B, size_t rows, size_t cols);
void MatrixAddIdentity(float *A, size_t rows, size_t cols);
void MatrixAccumulate(float *A, float *B, size_t rows, size_t cols);

// Subtraction
void MatrixSubtract(float *C, float *A, float *B, size_t rows, size_t cols);
void MatrixSubtractIdentity(float *A, size_t rows, size_t cols);
void MatrixSubAccumulate(float *A, float *B, size_t rows, size_t cols);
void MatrixNegate(float *A, size_t rows, size_t cols);

// Multiplication
void MatrixVectorMult(float *outVec, float *A, float *b, size_t rows, size_t cols);
void MatrixMultiply(float *C, float *A, float *B, size_t aRows, size_t aCols, size_t bRows, size_t bCols);
void MatrixMultiply_ABt(float *C, float *A, float *B, size_t arows, size_t acols, size_t brows);

// Invert
bool MatrixInverseCholesky(float *A, size_t n);
bool _MatrixCholeskyDecomp(float *A, size_t n);
bool _MatrixLowerTriangularInverse(float *A, size_t n);
// bool MatrixIsPosDef(float *A, size_t rows, size_t cols);

// #ifdef MATRIX_MATH_DEBUG
    // void PrintVector(float *a, size_t n);
    // void PrintMatrix(float *A, size_t r, size_t c);
// #endif