#include <unity.h>
#include <Wire.h>
#include <Arduino.h>
#include "hummingbird_config.h"
// #include <math.h>
#include "maths/vectors.h"
// #include "maths/matrices.h"
// #include "maths/matrix_math.h"
// #include "maths/math_functs.h"

// #include "vectors_test.h"


// default vector's initial values
void vectorf_default_vals(void)
{
    Vectorf vec;
    float testarr[3] = {0.0f, 0.0f, 0.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(testarr, vec.vec, 3);
}







void setup()
{
    delay(5000);  // service delay

    UNITY_BEGIN();
    

    RUN_TEST(vectorf_default_vals);

    UNITY_END();
    //
}

void loop()
{
    //
}