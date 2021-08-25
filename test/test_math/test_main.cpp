// ----------------------------------------------------------------------------
// MATH LIBRARY UNIT TESTS
// 
// Code By: Michael Wrona
// Created: 25 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests for the HFCU's basic math functions.
 */

#ifdef UNIT_TEST
#include <math.h>
#include <unity.h>
#include <Arduino.h>
#include "maths/math_functs.h"


/* Test the fast inverse square root algorithm */
void test_math_InvSqrtf(void)
{
    float expected;
    float actual;
    float delta;

    // compute 1/sqrt(4192.94)
    expected = 0.0154433201983094839201319974345f;
    actual = InvSqrtf(4192.94);
    
    // see if the result is within 0.175% of the expected
    // this is the max. expected error of the Newton-Rhapson iteration
    // https://medium.com/hard-mode/the-legendary-fast-inverse-square-root-e51fee3b49d9#:~:text=maximum%20error%20of%200.175%25
    delta = 0.00175f * expected;
    
    // Test if the result is within
    TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual);
}


/* Test range constrain above bounds*/
void test_math_RangeConstrain_upper(void)
{
    float lower;
    float upper;
    float val;
    float res;

    lower = 10.0f;
    upper = 20.0f;
    val = 34.0f;
    
    res = RangeConstrain(val, lower, upper);
    TEST_ASSERT_EQUAL(upper, res);
}


/* Test range constrain below bounds*/
void test_math_RangeConstrain_lower(void)
{
    float lower;
    float upper;
    float val;
    float res;

    lower = 10.0f;
    upper = 20.0f;
    val = 8.0f;
    
    res = RangeConstrain(val, lower, upper);
    TEST_ASSERT_EQUAL(lower, res);
}


/* Test range constrain within bounds*/
void test_math_RangeConstrain_within(void)
{
    float lower;
    float upper;
    float val;
    float res;

    lower = 10.0f;
    upper = 20.0f;
    val = 16.5f;
    
    res = RangeConstrain(val, lower, upper);
    TEST_ASSERT_EQUAL(val, res);
}


/* Test safe square root in normal case */
void test_math_sqrtf_safe_normal(void)
{
    float expected;
    float actual;

    // Test with sqrt(2)
    expected = 1.4142135623730950488016887242097f;
    actual = sqrtf_safe(2.0f);

    TEST_ASSERT_EQUAL_FLOAT(expected, actual);
}


/* Test safe square root with negative input */
void test_math_sqrtf_safe_neg_input(void)
{
    float actual;

    // Test with sqrt(-2), should return zero
    actual = sqrtf_safe(-2.0f);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, actual);
}


/* Test safe arcsine with a value greater than 1 */
void test_math_asinf_safe_invalid(void)
{
    float actual;
    float expected;

    // asinf_safe(1.5) should return pi/2
    expected = 1.5707963267948966192313216916398f;
    actual = asinf_safe(1.5);

    TEST_ASSERT_EQUAL_FLOAT(expected, actual);
}



void run_tests()
{
    delay(5000);
    UNITY_BEGIN();

    RUN_TEST(test_math_InvSqrtf);
    RUN_TEST(test_math_RangeConstrain_lower);
    RUN_TEST(test_math_RangeConstrain_upper);
    RUN_TEST(test_math_RangeConstrain_within);
    RUN_TEST(test_math_sqrtf_safe_normal);
    RUN_TEST(test_math_sqrtf_safe_neg_input);
    RUN_TEST(test_math_asinf_safe_invalid);

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
    // loop code
}
#else
int main(int argc, char **argv)
{
    run_tests();
    return 0;
}
#endif
#endif
