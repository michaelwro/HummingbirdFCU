// ----------------------------------------------------------------------------
// VECTOR OBJECT UNIT TESTS
//
// Code By: Michael Wrona
// Created: 23 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests for vector objects.
 */



#include "vector_tests.h"

// default vector's length
void test_vectorf_default_len(void)
{
    Vectorf vec;
    TEST_ASSERT_EQUAL((size_t)3, vec.len);
}

// default vector's initial values
void test_vectorf_default_vals(void)
{
    Vectorf vec;
    float testarr[3] = {0.0f, 0.0f, 0.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(testarr, vec.vec, 3);
}
