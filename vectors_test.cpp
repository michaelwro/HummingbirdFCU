/**
 * UNIT TEST FUNCTIONS FOR VECTORS OBJECT
 */


#include "vectors_test.h"


// default vector's length
void vectorf_default_len(void)
{
    Vectorf vec;
    TEST_ASSERT_EQUAL((size_t)3, vec.len);
}

// default vector's initial values
void vectorf_default_vals(void)
{
    Vectorf vec;
    float testarr[3] = {0.0f, 0.0f, 0.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(testarr, vec.vec, 3);
}



