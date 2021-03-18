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

#if defined(DEBUG) && defined(DEBUG_PORT)
/* Enable a message to signal when a vector was created and destroyed */
// #define VECTOR_OBJ_DEBUG
#endif


/**
 * Macro to extract the value in vector "VEC" at "VEC(i)" with length "len." 
 * This macro makes array/vector indexing a bit easier and consistent 
 * with the MATRIX_ELEMENT macro. IT IS UP TO THE USER TO ENSURE THAT THE INDEX 
 * IS LESS THAN THE VECTOR LENGTH!
 * 
 * @param VEC   Vectorf variable name
 * @param i     Vectorf index
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
class Vectorf {
public:
    // --------------------------------------------------------------------
    // Vectorf(size_t len)
    // --------------------------------------------------------------------
    /**
     * Define a vector by it's rows/length. The vector is allocated on the 
     * heap with the C++ 'new' keyword (RAM2 on the Teensy 4.1).
     * 
     * @param len   Length of the vector, # of elements. Default = 3
     */
    Vectorf(size_t len = 3)
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
            DEBUG_PORT.print("Created "); DEBUG_PORT.print(this->len);
            DEBUG_PORT.print("x1"); DEBUG_PORT.println(" vector");
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
     * Print vector to the debug port. Can only be used if DEBUG_PORT and
     * VECTOR_OBJ_DEBUG are defined.
     */
    void PrintToDebug(size_t decimals = 4)
    {
        size_t i;

        // Cannot print more than 7 decimals for floats...
        if (decimals >= 7)
            decimals = 4;

        DEBUG_PORT.print("[");
        for (i = 0; i < this->len; i++)
        {
            DEBUG_PORT.print(VECTOR_ELEMENT(this->vec, i), decimals);
            DEBUG_PORT.print((i != this->len - 1) ? ", " : " ");
        }
        DEBUG_PORT.println("]");
        DEBUG_PORT.println("");  // Newline to help separate vectors
    }
    #endif
    
    
    /* VARIABLES */
    size_t len;  // Length of the vector, # of elements
    float *vec;  // Pointer to the array/vector


    /*  Deconstruct/deallocate the vector once it goes out-of-scope.  */
    ~Vectorf()
    {
        delete[] this->vec;

        #ifdef VECTOR_OBJ_DEBUG
            DEBUG_PORT.print("Deallocated "); DEBUG_PORT.print(this->len);
            DEBUG_PORT.print("x1"); DEBUG_PORT.println(" vector");
        #endif
    }
protected:
private:
};


/**
 * A vector object is definied by it's rows/length. When a vector object 
 * is created, the array is allocated on the heap (RAM2 for Teensy 4.1) with 
 * the C++ 'new' keyword. See the following resource to learn more: 
 * https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/
 * 
 * @param len   Length of the vector, # of elements. Default = 3
 */
class Vectord {
public:
    // --------------------------------------------------------------------
    // Vectord(size_t len)
    // --------------------------------------------------------------------
    /**
     * Define a vector by it's rows/length. The vector is allocated on the 
     * heap with the C++ 'new' keyword (RAM2 on the Teensy 4.1).
     * 
     * @param len   Length of the vector, # of elements. Default = 3
     */
    Vectord(size_t len = 3)
    {
        size_t i;
        double initVal = 0.0;  // Value to initialize and fill array with

        this->len = len;

        // Create the array on the heap (RAM2 on Teensy 4.1)
        this->vec = new double[this->len];
        
        // Init. values
        for (i = 0; i < this->len; i++)
            this->vec[i] = initVal;


        #ifdef VECTOR_OBJ_DEBUG
            DEBUG_PORT.print("Created "); DEBUG_PORT.print(this->len);
            DEBUG_PORT.print("x1"); DEBUG_PORT.println(" vector");
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
    double GetNorm()
    {
        size_t i;
        double sumsq;
        double vecNorm;

        for (i = 0; i < this->len; i++)
            sumsq += (this->vec[i] * this->vec[i]);
        
        vecNorm = sqrt(sumsq);
        
        return vecNorm;
    }


    #ifdef VECTOR_OBJ_DEBUG
    /**
     * Print vector to the debug port. Can only be used if DEBUG_PORT and
     * VECTOR_OBJ_DEBUG are defined.
     */
    void PrintToDebug(size_t decimals = 4)
    {
        size_t i;

        DEBUG_PORT.print("[");
        for (i = 0; i < this->len; i++)
        {
            DEBUG_PORT.print(VECTOR_ELEMENT(this->vec, i), decimals);
            DEBUG_PORT.print((i != this->len - 1) ? ", " : " ");
        }
        DEBUG_PORT.println("]");
        DEBUG_PORT.println("");  // Newline to help separate vectors
    }
    #endif
    
    
    /* VARIABLES */
    size_t len;  // Length of the vector, # of elements
    double *vec;  // Pointer to the array/vector


    /*  Deconstruct/deallocate the vector once it goes out-of-scope.  */
    ~Vectord()
    {
        delete[] this->vec;

        #ifdef VECTOR_OBJ_DEBUG
            DEBUG_PORT.print("Deallocated "); DEBUG_PORT.print(this->len);
            DEBUG_PORT.print("x1"); DEBUG_PORT.println(" vector");
        #endif
    }
protected:
private:
};
