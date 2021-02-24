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
 */

#pragma once

#include <Arduino.h>
#include "hummingbird_config.h"


/* Enable a message to signal when a matrix was created and destroyed */
#define MATRIX_OBJ_DEBUG

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

        #if defined(DebugPort) && defined(MATRIX_OBJ_DEBUG)
            DebugPort.print("Created "); DebugPort.print(this->rows); DebugPort.print("x");
            DebugPort.print(this->cols); DebugPort.println(" matrix");
        #endif
    }


    #if defined(DebugPort) && defined(MATRIX_OBJ_DEBUG)
    /**
     * Print matrix to the debug port. Can only be used if DebugPort and
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
            DebugPort.print("[[ ");
            for (j = 0; j < this->cols; j++)
            {
                DebugPort.print(MATRIX_ELEMENT(this->mat, i, j, this->rows, this->cols), decimals);
                DebugPort.print((j != this->cols - 1) ? ", " : "");
            }
            DebugPort.println(" ]]");
        }
        DebugPort.println("");  // Newline to help separate matrices
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

        #if defined(DebugPort) && defined(MATRIX_OBJ_DEBUG)
            DebugPort.print("Deallocated "); DebugPort.print(this->rows); DebugPort.print("x");
            DebugPort.print(this->cols); DebugPort.println(" matrix");
        #endif
    }
protected:
private:
};
