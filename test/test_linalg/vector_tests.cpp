// ----------------------------------------------------------------------------
// VECTOR OBJECT UNIT TESTS
//
// Code By: Michael Wrona
// Created: 23 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests for vector objects.
 */


#ifdef UNIT_TEST
#include "vector_tests.h"

// default vector's length
void test_vectorf_default_len(void)
{
    Vectorf vec;
    TEST_ASSERT_EQUAL((size_t)3, vec.len);
}

void test_vectord_default_len(void)
{
    Vectord vec;
    TEST_ASSERT_EQUAL((size_t)3, vec.len);
}


// default vector's initial values
void test_vectorf_default_vals(void)
{
    Vectorf vec;
    float testarr[3] = {0.0f, 0.0f, 0.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(testarr, vec.vec, 3);
}

void test_vectord_default_vals(void)
{
    Vectord vec;
    double testarr[3] = {0.0, 0.0, 0.0};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(testarr, vec.vec, 3);
}


// vector norm
void test_vectorf_norm(void)
{
    Vectorf vec(4);
    vec.vec[0] = 5.0f;
    vec.vec[1] = 5.0f;
    vec.vec[2] = 5.0f;
    vec.vec[3] = 5.0f;
    TEST_ASSERT_EQUAL_FLOAT(10.0f, vec.GetNorm());
}

void test_vectord_norm(void)
{
    Vectord vec(4);
    vec.vec[0] = 5.0;
    vec.vec[1] = 5.0;
    vec.vec[2] = 5.0;
    vec.vec[3] = 5.0;
    TEST_ASSERT_EQUAL_FLOAT(10.0, vec.GetNorm());
}

#endif
