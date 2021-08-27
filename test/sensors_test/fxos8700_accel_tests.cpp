// ----------------------------------------------------------------------------
// FXOS8700 ACCELEROMETER UNIT TESTS
// 
// Code By: Michael Wrona
// Created: 27 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests to test the functionality and health of the FXOS8700 
 * accelerometer sensor. These make sure we can connect to the sensor and if it 
 * can be configured properly. This also puts the sensor into self-test mode to 
 * verify the signal chain functionality (p.8 in datasheet).
 */


#ifdef UNIT_TEST
#include "fxos8700_accel_tests.h"



/* Check if the measurement range can be set to 2G */
void test_fxos8700_init_2g(void)
{
    bool status;

    FXOS8700AccelMag accel(&SENSOR_I2C);

    status = accel.Initialize(ACCEL_RNG_2G);
    TEST_ASSERT_TRUE(status);
}


/* Check if the measurement range can be set to 4G */
void test_fxos8700_init_4g(void)
{
    bool status;

    FXOS8700AccelMag accel(&SENSOR_I2C);

    status = accel.Initialize(ACCEL_RNG_4G);
    TEST_ASSERT_TRUE(status);
}


/* Check if the measurement range can be set to 8G */
void test_fxos8700_init_8g(void)
{
    bool status;

    FXOS8700AccelMag accel(&SENSOR_I2C);

    status = accel.Initialize(ACCEL_RNG_8G);
    TEST_ASSERT_TRUE(status);
}


/* Put the sensor into self-test mode and make sure the signal chain works properly */
void test_fxos8700_self_test(void)
{
    int16_t axThres = 192;  // [LSB] ax self-test output range
    int16_t ayThres = 270;  // [LSB] ay self-test output range
    int16_t azThres = 1275;  // [LSB] az self-test output range

    // set to standby mode to make config. changes
    SENSOR_I2C.begin();
    FXOS8700_I2Cwrite8(&SENSOR_I2C, ACCELMAG_REG_CTRL1, 0x00);

    // Put into +/- 2G measurement mode
    FXOS8700_I2Cwrite8(&SENSOR_I2C, ACCELMAG_REG_XYZ_CFG, 0x00);

    // Put into self-test mode
    FXOS8700_I2Cwrite8(&SENSOR_I2C, ACCELMAG_REG_CTRL2, 0x80);  // Self-test enabled. Reset disabled. Sleep mode OSR mode = normal. Wake mode OSR mode = normal.

    // Sleep mode ODR = 50Hz. Sensor ODR = 400Hz single sensor = 200Hz hybrid mode. Full-scale range mode = low-noise mode. Fast read mode = normal. Active mode.
    FXOS8700_I2Cwrite8(&SENSOR_I2C, ACCELMAG_REG_CTRL1, 0x0D);

    // Read data
    SENSOR_I2C.beginTransmission((uint8_t)FXOS8700_ADDRESS);
    SENSOR_I2C.write(ACCELMAG_REG_STATUS | 0x80);
    SENSOR_I2C.endTransmission();
    SENSOR_I2C.requestFrom((uint8_t)FXOS8700_ADDRESS, (uint8_t)7);

    uint8_t status = SENSOR_I2C.read();
    uint8_t axhi = SENSOR_I2C.read();
    uint8_t axlo = SENSOR_I2C.read();
    uint8_t ayhi = SENSOR_I2C.read();
    uint8_t aylo = SENSOR_I2C.read();
    uint8_t azhi = SENSOR_I2C.read();
    uint8_t azlo = SENSOR_I2C.read();

    int16_t axRaw = (int16_t)((axhi << 8) | axlo) >> 2;
    int16_t ayRaw = (int16_t)((ayhi << 8) | aylo) >> 2;
    int16_t azRaw = (int16_t)((azhi << 8) | azlo) >> 2;

    // perform tests
    TEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(axThres, abs(axRaw), "[ERROR]: Less than expected Ax self-test.");
    TEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(ayThres, abs(ayRaw), "[ERROR]: Less than expected Ay self-test.");
    TEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(azThres, abs(azRaw), "[ERROR]: Less than expected Az self-test.");
}







void FXOS8700_I2Cwrite8(TwoWire *sensorWire, uint8_t regOfInterest, uint8_t valToWrite)
{
    // Init. communication
    sensorWire->beginTransmission(FXOS8700_ADDRESS);
    sensorWire->write((uint8_t)regOfInterest);
    sensorWire->write((uint8_t)valToWrite);
    sensorWire->endTransmission();
}


uint8_t FXOS8700_I2Cread8(TwoWire *sensorWire, uint8_t regOfInterest)
{
    uint8_t val;

    // Init. communication
    sensorWire->beginTransmission((uint8_t)FXOS8700_ADDRESS);
    sensorWire->write((uint8_t)regOfInterest);

    // Check for failure
    if (sensorWire->endTransmission(false) != 0) return 0;
    
    // Read register
    sensorWire->requestFrom((uint8_t)FXOS8700_ADDRESS, (uint8_t)1);
    val = sensorWire->read();

    return val;
}


#endif  // UNIT_TEST
