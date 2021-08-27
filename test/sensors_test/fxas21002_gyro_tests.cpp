// ----------------------------------------------------------------------------
// FXAS21002 GYRO UNIT TESTS
//
// Code By: Michael Wrona
// Created: 27 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit test to make sure we can connect to the FXAS21002 gyro sensor 
 * and that it is working properly.
 */


#ifdef UNIT_TEST
#include "fxas21002_gyro_tests.h"



/* Test setting measurement range to 250dps */
void test_fxas21002_init_250dps(void)
{
    bool status;
    FXAS21002Gyro gyro(&SENSOR_I2C);

    // sensor initialization should return true
    status = gyro.Initialize(GYRO_RNG_250DPS);
    TEST_ASSERT_TRUE(status);
}


/* Test setting measurement range to 500dps */
void test_fxas21002_init_500dps(void)
{
    bool status;
    FXAS21002Gyro gyro(&SENSOR_I2C);

    // sensor initialization should return true
    status = gyro.Initialize(GYRO_RNG_500DPS);
    TEST_ASSERT_TRUE(status);
}


/* Test setting measurement range to 1000dps */
void test_fxas21002_init_1000dps(void)
{
    bool status;
    FXAS21002Gyro gyro(&SENSOR_I2C);

    // sensor initialization should return true
    status = gyro.Initialize(GYRO_RNG_1000DPS);
    TEST_ASSERT_TRUE(status);
}


/* Test setting measurement range to 2000dps */
void test_fxas21002_init_2000dps(void)
{
    bool status;
    FXAS21002Gyro gyro(&SENSOR_I2C);

    // sensor initialization should return true
    status = gyro.Initialize(GYRO_RNG_2000DPS);
    TEST_ASSERT_TRUE(status);
}


/* Put the gyro in self-test mode and test the ASIC DSP pipeline */
void test_fxas21002_self_test(void)
{
    int16_t lowerBound = 7000;  // Min LSB in self-test mode
    int16_t upperBound = 25000;  // Max LSB in self-test mode

    // put sensor into self-test mode
    SENSOR_I2C.begin();
    // reg0 -> p.39
    // reg1 -> p.45
    FXAS21002_I2Cwrite8(&SENSOR_I2C, GYRO_REG_CTRL1, 0x00);  // Put into standby mode
    FXAS21002_I2Cwrite8(&SENSOR_I2C, GYRO_REG_CTRL1, (1 << 6));  // Reset
    FXAS21002_I2Cwrite8(&SENSOR_I2C, GYRO_REG_CTRL0, 0x00);  // CTRL_REG0[FS] = 00 (2000dps)
    FXAS21002_I2Cwrite8(&SENSOR_I2C, GYRO_REG_CTRL1, 0x22);  // Enable self-test. 800Hz DR. Active.
    delay(100);
    
    // Read 7 bytes from sensor
    SENSOR_I2C.beginTransmission((uint8_t)FXAS21002C_ADDRESS);
    SENSOR_I2C.write(GYRO_REG_STATUS | 0x80);
    SENSOR_I2C.endTransmission();
    SENSOR_I2C.requestFrom((uint8_t)FXAS21002C_ADDRESS, (uint8_t)7);

    uint8_t status = SENSOR_I2C.read();
    uint8_t xhi = SENSOR_I2C.read();
    uint8_t xlo = SENSOR_I2C.read();
    uint8_t yhi = SENSOR_I2C.read();
    uint8_t ylo = SENSOR_I2C.read();
    uint8_t zhi = SENSOR_I2C.read();
    uint8_t zlo = SENSOR_I2C.read();

    // Shift values to make proper integer
    int16_t gxRaw = (int16_t)((xhi << 8) | xlo);
    int16_t gyRaw = (int16_t)((yhi << 8) | ylo);
    int16_t gzRaw = (int16_t)((zhi << 8) | zlo);

    // Check that each reading is within bounds.
    TEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(lowerBound, abs(gxRaw), "[ERROR]: Gx lower bound check");
    TEST_ASSERT_LESS_OR_EQUAL_INT16_MESSAGE(upperBound, abs(gxRaw), "[ERROR]: Gx upper bound check");

    TEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(lowerBound, abs(gyRaw), "[ERROR]: Gy lower bound check");
    TEST_ASSERT_LESS_OR_EQUAL_INT16_MESSAGE(upperBound, abs(gyRaw), "[ERROR]: Gy upper bound check");

    TEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(lowerBound, abs(gzRaw), "[ERROR]: Gz lower bound check");
    TEST_ASSERT_LESS_OR_EQUAL_INT16_MESSAGE(upperBound, abs(gzRaw), "[ERROR]: Gz upper bound check");
}

// void test_fxas21002_init_selftest_gx(void)
// {

// }







void FXAS21002_I2Cwrite8(TwoWire *sensorWire, uint8_t regOfInterest, uint8_t valToWrite)
{
    // Init. communication
    sensorWire->beginTransmission(FXAS21002C_ADDRESS);
    sensorWire->write((uint8_t)regOfInterest);
    sensorWire->write((uint8_t)valToWrite);
    sensorWire->endTransmission();
}


uint8_t FXAS21002_I2Cread8(TwoWire *sensorWire, uint8_t regOfInterest)
{
    uint8_t val;

    // Init. communication
    sensorWire->beginTransmission((uint8_t)FXAS21002C_ADDRESS);
    sensorWire->write((uint8_t)regOfInterest);

    // Check for failure
    if (sensorWire->endTransmission(false) != 0)
        return 0;
    
    // Read register
    sensorWire->requestFrom((uint8_t)FXAS21002C_ADDRESS, (uint8_t)1);
    val = sensorWire->read();

    return val;
}


#endif  // UNIT_TEST
