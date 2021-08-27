// ----------------------------------------------------------------------------
// SENSOR UNIT TESTS
// 
// Code By: Michael Wrona
// Created: 27 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests to test the FXAS21002 gyro, FXOS8700 
 * accelerometer/magnetometer, LIS3MDL magnetometer, and BMP388 
 * temperature/pressure sensors. These unit tests make sure that the FCU can 
 * connect to the sensors over I2C and function properly. Unit tests also put 
 * the devices into self-test mode to ensure their signal processing chain 
 * (ADC's, amps, etc.) is working properly.
 */


#ifdef UNIT_TEST
#include <unity.h>
#include <Arduino.h>
#include "hummingbird_config.h"
#include "fxas21002_gyro_tests.h"

/* Set tests to run */
// #define TEST_GYRO_SENSOR


void run_tests()
{
    delay(5000);
    UNITY_BEGIN();

    RUN_TEST(test_fxas21002_init_250dps);
    RUN_TEST(test_fxas21002_init_500dps);
    RUN_TEST(test_fxas21002_init_1000dps);
    RUN_TEST(test_fxas21002_init_2000dps);
    RUN_TEST(test_fxas21002_self_test);

    UNITY_END();
}




#ifdef ARDUINO
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
#endif  // ARDUINO
#endif  // UNIT_TEST
