// ----------------------------------------------------------------------------
// VECTOR OBJECT DEFINITION
// 
// Created By: Michael Wrona
// Created: 16 Feb 2021
// ----------------------------------------------------------------------------
/**
 * A vector object is definied by it's rows/length. When a vector object 
 * is created, the array is allocated on the heap (RAM2 for Teensy 4.1) with 
 * the C++ 'new' keyword. See the following resource to learn more: 
 * https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/
 */

#pragma once

#include <Arduino.h>
#include "hummingbird_config.h"

#if defined(DEBUG) && defined(DebugPort)
    /* Enable a message to signal when a vector was created and destroyed */
    // #define VECTOR_OBJ_DEBUG
#endif

/**
 * Macro to extract the value in vector "VEC" at "VEC(i)" with length "len." 
 * This macro makes array/vector indexing a bit easier and consistent 
 * with the MATRIX_ELEMENT macro. IT IS UP TO THE USER TO ENSURE THAT THE INDEX 
 * IS LESS THAN THE VECTOR LENGTH!
 * 
 * @param VEC   Vector variable name
 * @param i     Vector index
 */
#define VECTOR_ELEMENT(VEC, i) VEC[i]
// #define VECTOR_ELEMENT(VEC, i) *(VEC + i)


/**
 * A vector object is definied by it's rows/length. When a vector object 
 * is created, the array is allocated on the heap (RAM2 for Teensy 4.1) with 
 * the C++ 'new' keyword. See the following resource to learn more: 
 * https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/
 * 
 * @param len   Length of the vector, # of elements. Default = 3
 */
class Vector {
public:
    // --------------------------------------------------------------------
    // Vector(size_t len)
    // --------------------------------------------------------------------
    /**
     * Define a vector by it's rows/length. The vector is allocated on the 
     * heap with the C++ 'new' keyword (RAM2 on the Teensy 4.1).
     * 
     * @param len   Length of the vector, # of elements. Default = 3
     */
    Vector(size_t len = 3)
    {
        size_t i;
        float initVal = 0.0f;  // Value to initialize and fill array with

        this->len = len;

        // Create the array on the heap (RAM2 on Teensy 4.1)
        this->vec = new float[this->len];
        
        // Init. values
        for (i = 0; i < this->len; i++)
            this->vec[i] = initVal;


        #ifdef VECTOR_OBJ_DEBUG
            DebugPort.print("Created "); DebugPort.print(this->len);
            DebugPort.print("x1"); DebugPort.println(" vector");
        #endif
    }

    // --------------------------------------------------------------------
    // GetNorm()
    // --------------------------------------------------------------------
    /**
     * Return the 2-norm (magnitude) of the vector.
     * 
     * @returns     Norm of the vector
     */
    float GetNorm()
    {
        size_t i;
        float sumsq;
        float vecNorm;

        for (i = 0; i < this->len; i++)
            sumsq += (this->vec[i] * this->vec[i]);
        
        vecNorm = sqrtf(sumsq);
        
        return vecNorm;
    }


    #ifdef VECTOR_OBJ_DEBUG
    /**
     * Print vector to the debug port. Can only be used if DebugPort and
     * VECTOR_OBJ_DEBUG are defined.
     */
    void PrintToDebug(size_t decimals = 4)
    {
        size_t i;

        // Cannot print more than 7 decimals for floats...
        if (decimals >= 7)
            decimals = 4;

        DebugPort.print("[");
        for (i = 0; i < this->len; i++)
        {
            DebugPort.print(VECTOR_ELEMENT(this->vec, i), decimals);
            DebugPort.print((i != this->len - 1) ? ", " : " ");
        }
        DebugPort.println("]");
        DebugPort.println("");  // Newline to help separate vectors
    }
    #endif
    
    
    /* VARIABLES */
    size_t len;  // Length of the vector, # of elements
    float *vec;  // Pointer to the array/vector


    /*  Deconstruct/deallocate the vector once it goes out-of-scope.  */
    ~Vector()
    {
        delete[] this->vec;

        #ifdef VECTOR_OBJ_DEBUG
            DebugPort.print("Deallocated "); DebugPort.print(this->len);
            DebugPort.print("x1"); DebugPort.println(" vector");
        #endif
    }
protected:
private:
};
