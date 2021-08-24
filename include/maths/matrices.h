// ----------------------------------------------------------------------------
// MATRIX OBJECT DEFINITION
// 
// Created By: Michael Wrona
// Created: 16 Feb 2021
// ----------------------------------------------------------------------------
/**
 * A matrix object is definied by it's rows and columns. When a matrix object 
 * is created, the array is allocated on the heap (RAM2 for Teensy 4.1) with 
 * the C++ 'new' keyword as an array of pointers. See the following resource to 
 * learn more.
 * 
 * https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/
 * https://dev.to/drakargx/c-contiguous-allocation-of-2-d-arrays-446m
 */

#pragma once

#include <Arduino.h>
#include "hummingbird_config.h"


#if defined(DEBUG) && defined(DEBUG_PORT)
    // #define MATRIX_OBJ_DEBUG  /* Enable a message to signal when a matrix was created and destroyed */
#endif

/**
 * Macro to extract the value in matrix "MAT" at "MAT(i, j)" with dims. 
 * "(rows, cols)." Matrices are stored as a long 1D array, so this macro makes 2D 
 * array indexing a bit easier. IT IS UP TO THE USER TO ENSURE THAT THE INDICES 
 * DO NOT EXCEED THE MATRIX DIMENSIONS!
 * 
 * @param MAT   Matrix variable name
 * @param i     Row index
 * @param j     Column index
 * @param rows  Number of rows in the matrix
 * @param cols  Number of cols in the matrix
 */
#define MATRIX_ELEMENT(MAT, i, j, rows, cols) *(MAT + i*cols + j)


/**
 * A matrix object is definied by it's rows and columns. When a matrix object 
 * is created, the array is allocated on the heap (RAM2 for Teensy 4.1) with 
 * the C++ 'new' keyword as an array of pointers. See the following resource to 
 * learn more: 
 * https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/
 * https://dev.to/drakargx/c-contiguous-allocation-of-2-d-arrays-446m
 * 
 * @param rows  Matrix rows. Default = 3
 * @param cols  Matrix columns. Default = 3
 */
class Matrix {
public:
    // ----------------------------------------------------------------------------
    // Matrix(size_t rows, size_t cols)
    // ----------------------------------------------------------------------------
    /**
     * Define a matrix by rows and columns. The matrix is allocated on the heap 
     * with the C++ 'new' keyword (RAM2 on the Teensy 4.1).
     * 
     * @param rows  Matrix rows. Default = 3
     * @param cols  Matrix columns. Default = 3
     */
    Matrix(size_t rows = 3, size_t cols = 3)
    {
        size_t i;
        float initVal = 0.0f;  // Value to initialize and fill array with
        float **pmat;  // pointer for construction

        this->rows = rows;
        this->cols = cols;

        // Create the array on the heap (RAM2 on Teensy 4.1)
        this->mat = new float[this->rows*this->cols];
        pmat = new float*[this->rows];
        for (i = 1, *pmat = this->mat; i < this->rows; i++)
            *(pmat + i) = *(pmat + i - 1) + this->cols;
        
        // Init. values
        size_t rc = this->rows * this->cols;
        for (i = 0; i < rc; i++)
            this->mat[i] = initVal;

    #ifdef MATRIX_OBJ_DEBUG
        DEBUG_PORT.print("Created "); DEBUG_PORT.print(this->rows); DEBUG_PORT.print("x");
        DEBUG_PORT.print(this->cols); DEBUG_PORT.println(" matrix");
    #endif
    }


    #ifdef MATRIX_OBJ_DEBUG
    /**
     * Print matrix to the debug port. Can only be used if DEBUG_PORT and
     * MATRIX_OBJ_DEBUG are defined.
     */
    void PrintToDebug(size_t decimals = 2)
    {
        size_t i, j;

        // Cannot print more than 7 decimals for floats...
        if (decimals >= 7)
            decimals = 4;
        
        for (i = 0; i < this->rows; i++)
        {
            DEBUG_PORT.print("[[ ");
            for (j = 0; j < this->cols; j++)
            {
                DEBUG_PORT.print(MATRIX_ELEMENT(this->mat, i, j, this->rows, this->cols), decimals);
                DEBUG_PORT.print((j != this->cols - 1) ? ", " : "");
            }
            DEBUG_PORT.println(" ]]");
        }
        DEBUG_PORT.println("");  // Newline to help separate matrices
    }
    #endif


    /* VARIABLES */
    size_t rows;  // Rows of the matrix
    size_t cols;  // Columns of the matrix
    float *mat;  // Pointer to the 2D array/matrix


    /*  Deconstruct/deallocate the matrix once it goes out-of-scope.  */
    ~Matrix()
    {
        delete[] this->mat;

    #ifdef MATRIX_OBJ_DEBUG
        DEBUG_PORT.print("Deallocated "); DEBUG_PORT.print(this->rows); DEBUG_PORT.print("x");
        DEBUG_PORT.print(this->cols); DEBUG_PORT.println(" matrix");
    #endif
    }
protected:
private:
};
