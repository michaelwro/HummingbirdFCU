// ----------------------------------------------------------------------------
// UNIT TESTS FOR THE HUMMINGBIRD FCU LINEAR ALGEBRA LIBRARIES
// Code By: Michael Wrona
// Created: 23 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These test float and double vector objects, float and double matrix objects, 
 * and matrix math library.
 */


#ifdef UNIT_TEST  // Use this if running unit tests and 'disable' src/main.cpp
#include <unity.h>
#include <Arduino.h>
#include "hummingbird_config.h"
#include "vector_tests.h"
#include "matrix_tests.h"
#include "matrix_math_tests.h"
#include "maths/matrix_math.h"


/* Enable/disable certain tests (comment/uncomment) */
#define TEST_VECTORF  // Test float vector class
#define TEST_VECTORD  // Test double vector class
#define TEST_MATRIX  // Test float matrix class
#define TEST_MATRIX_MATH  // Test matrix math functions


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

    // Matrix math & linear algebra lib tests
    #ifdef TEST_MATRIX_MATH
    RUN_TEST(test_linalg_VectorfFill);
    RUN_TEST(test_linalg_VectorfAdd);
    RUN_TEST(test_linalg_VectorfAccumulate);
    RUN_TEST(test_linalg_VectorfSubtract);
    RUN_TEST(test_linalg_MatrixFill);
    RUN_TEST(test_linalg_MatrixAdd);
    RUN_TEST(test_linalg_MatrixSubtract);
    RUN_TEST(test_linalg_MatrixVectorfMult);
    RUN_TEST(test_linalg_MatrixMultiply);
    RUN_TEST(test_linalg_MatrixCholeskyDecomp);
    RUN_TEST(test_linalg_MatrixInverseCholesky);
    #endif

    UNITY_END();
}


#ifdef ARDUINO
void setup()
{
    pinMode(RED_LED, OUTPUT);
    pinMode(GRN_LED, OUTPUT);
    
    // Red during tests
    digitalWrite(GRN_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    run_tests();

    // green after tests
    digitalWrite(RED_LED, LOW);
    digitalWrite(GRN_LED, HIGH);

}

void loop()
{
    // code
}

#else
int main(int argc, char **argv)
{
    run_tests();
    return 0;
}

#endif
#endif