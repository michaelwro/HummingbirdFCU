// ----------------------------------------------------------------------------
// UNIT TESTS FOR THE HUMMINGBIRD FCU LINEAR ALGEBRA LIBRARIES
// Code By: Michael Wrona
// Created: 23 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These test float and double vector objects, float and double matrix objects, 
 * and matrix math library.
 */



#include <unity.h>
#include <Arduino.h>
#include "vector_tests.h"
#include "matrix_tests.h"


/* Enable/disable certain tests (comment/uncomment) */
#define TEST_VECTORF  // Test float vector class
#define TEST_VECTORD  // Test double vector class
#define TEST_MATRIX  // Test float matrix class
// #define TEST_MATRIX_MATH  // Test matrix math functions


void run_tests()
{
    delay(5000);  // service delay
    UNITY_BEGIN();

    // Vectorf tests
    #ifdef TEST_VECTORF
    RUN_TEST(test_vectorf_default_len);
    RUN_TEST(test_vectorf_default_vals);
    RUN_TEST(test_vectorf_norm);
    #endif

    // Vectord tests
    #ifdef TEST_VECTORD
    RUN_TEST(test_vectord_default_len);
    RUN_TEST(test_vectord_default_vals);
    RUN_TEST(test_vectord_norm);
    #endif

    // Matrix tests
    #ifdef TEST_MATRIX
    RUN_TEST(test_matrix_default_rows);
    RUN_TEST(test_matrix_default_rows);
    RUN_TEST(test_matrix_default_cols);
    RUN_TEST(test_matrix_default_vals);
    #endif

    UNITY_END();
}


#ifdef ARDUINO
#include <Arduino.h>
void setup()
{
    run_tests();
}

void loop()
{
    //
}

#else
int main(int argc, char **argv)
{
    run_tests();
    return 0;
}

#endif
