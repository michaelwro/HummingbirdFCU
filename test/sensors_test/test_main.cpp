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
#include "fxos8700_accel_tests.h"

/* Set tests to run */
#define TEST_SENSOR_FXAS21002  // gyro
#define TEST_SENSOR_FXOS8700  // accelerometer

void run_tests()
{
    delay(5000);
    UNITY_BEGIN();

    #ifdef TEST_SENSOR_GYRO
    RUN_TEST(test_fxas21002_init_250dps);
    RUN_TEST(test_fxas21002_init_500dps);
    RUN_TEST(test_fxas21002_init_1000dps);
    RUN_TEST(test_fxas21002_init_2000dps);
    RUN_TEST(test_fxas21002_self_test);
    #endif

    #ifdef TEST_SENSOR_FXOS8700
    RUN_TEST(test_fxos8700_init_2g);
    RUN_TEST(test_fxos8700_init_4g);
    RUN_TEST(test_fxos8700_init_8g);
    RUN_TEST(test_fxos8700_self_test);
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
