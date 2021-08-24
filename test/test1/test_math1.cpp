// ----------------------------------------------------------------------------
// HUMMINGBIRD FCU UNIT TESTS
// 
// Code By: Michael Wrona
// Created: 23 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests to test the Hummingbird FCU
 * 
 */

#include <unity.h>
#include <Wire.h>
#include <Arduino.h>
#include "hummingbird_config.h"
// #include <math.h>
#include "maths/vectors.h"
// #include "maths/matrices.h"
// #include "maths/matrix_math.h"
// #include "maths/math_functs.h"



// default vector's length
void vectorf_default_len(void)
{
    Vectorf vec;
    TEST_ASSERT_EQUAL((size_t)3, vec.len);
}







void setup()
{
    delay(5000);  // service delay

    UNITY_BEGIN();
    

    RUN_TEST(vectorf_default_len);

    UNITY_END();
    //
}

void loop()
{
    //
}

