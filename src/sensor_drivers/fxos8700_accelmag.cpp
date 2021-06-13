// ----------------------------------------------------------------------------
// FXOS8700 ACCELEROMETER & MAGNETOMETER SENSOR LIBRARY
// 
// Code By: Michael Wrona
// Created: 31 Jan 2021
// ----------------------------------------------------------------------------
/**
 * Source code for the FXOS8700 accelerometer and magnetometer sensor library.
 */



#include <Arduino.h>
#include <Wire.h>
// #include <limits.h>
#include "sensor_drivers/fxos8700_accelmag.h"


// ------------------------------------
// Public methods
// ------------------------------------

// ----------------------------------------------------------------------------
// FXOS8700AccelMag(int32_t accelID, int32_t magID)
// ----------------------------------------------------------------------------
/**
 * Constructor for the FXOS8700 Accelerometer/Magnetometer class. Be sure to
 * specify the sensor ID's.
 * accelID = 0x8700A
 * magID = 0x8700B
 * 
 * @param accelID  I2C address of the accelerometer
 * @param magID    I2C address of the magnetometer
 */
FXOS8700AccelMag::FXOS8700AccelMag(TwoWire *wireInput)
{
    this->_ax = 0.0f;  // Zero out variables
    this->_ay = 0.0f;
    this->_az = -CONSTS_GRAV;
    this->_mx = 0.0f;
    this->_my = 0.0f;
    this->_mz = 0.0f;
    // this->accelRangeCheck = 0.0f;
    // this->magRangeCheck = 1200.0f;  // 1200uT from datasheet
    this->prevMeasMicros = micros();
    this->_SensorWire = wireInput;
}


// ----------------------------------------------------------------------------
// Initialize(AccelRanges_t accRange)
// ----------------------------------------------------------------------------
/**
 * Initialize accelerometer, set accel. measurement range, configure
 * magnetometer. ACCEL_RNG_2G is default if the accelerometer measurement range
 * is not specified.
 * 
 * @param accRange  Desired accelerometer measurement range (ACCEL_RNG_2G,
 *                  ACCEL_RNG_4G, ACCEL_RNG_8G)
 */
bool FXOS8700AccelMag::Initialize(AccelRanges_t accRange)
{
    uint8_t connectedSensorID;

    this->_SensorWire->begin();  // Init. communication
    this->accelRange = accRange; // Set accelerometer range
    
    /* Check to make sure the ID register on the sensor matches the
    expected FXOS8700 ID. */
    connectedSensorID = this->I2Cread8(ACCELMAG_REG_ID);
    
    if (connectedSensorID != FXOS8700_ID)
        return false;
    
    /* Set sensor to STANDBY MODE in order to make changes to/configure
    a few registers. */
    this->I2Cwrite8(ACCELMAG_REG_CTRL1, 0);
    
    // Begin configuration
    switch (this->accelRange)  // Set accel. measurement range
    {
        case (ACCEL_RNG_2G):
            this->I2Cwrite8(ACCELMAG_REG_XYZ_CFG, 0x00);
            // this->accelRangeCheck = 2.0f;
            break;
        case (ACCEL_RNG_4G):
            this->I2Cwrite8(ACCELMAG_REG_XYZ_CFG, 0x01);
            // this->accelRangeCheck = 4.0f;
            break;
        case (ACCEL_RNG_8G):
            this->I2Cwrite8(ACCELMAG_REG_XYZ_CFG, 0x02);
            // this->accelRangeCheck = 8.0f;
            break;
        default:
            return false;
            break;
    }

    // Configure accel.
    this->I2Cwrite8(ACCELMAG_REG_CTRL2, 0x12);  // High resolution
    // this->I2Cwrite8(ACCELMAG_REG_CTRL1, 0x15);  // Active, Normal Mode, Low Noise, 100Hz in Hybrid Mode
    this->I2Cwrite8(ACCELMAG_REG_CTRL1, 0x05);  // Active, Normal Mode, Low Noise, 200Hz in Hybrid Mode

    // Configure magnetometer
    this->I2Cwrite8(ACCELMAG_REG_MCTRL1, 0x13);  // Hybrid Mode, Over Sampling Rate = 1
    this->I2Cwrite8(ACCELMAG_REG_MCTRL2, 0x20);  // Jump to reg 0x33 after reading 0x06

    return true;
}


// ----------------------------------------------------------------------------
// ReadSensor(void)
// ----------------------------------------------------------------------------
/**
 * Read acceleration and magnetometer data from the FXOS8700 sensor registers.
 * If defined, apply sensor calibration paramerters.
 * 
 * @return  True if successful
 */
bool FXOS8700AccelMag::ReadSensor()
{
    // Read 13 bytes from sensor
    this->_SensorWire->beginTransmission((uint8_t)FXOS8700_ADDRESS);
    this->_SensorWire->write(ACCELMAG_REG_STATUS | 0x80);
    this->_SensorWire->endTransmission();
    this->_SensorWire->requestFrom((uint8_t)FXOS8700_ADDRESS, (uint8_t)13);

    
    uint8_t status = this->_SensorWire->read();
    uint8_t axhi = this->_SensorWire->read();
    uint8_t axlo = this->_SensorWire->read();
    uint8_t ayhi = this->_SensorWire->read();
    uint8_t aylo = this->_SensorWire->read();
    uint8_t azhi = this->_SensorWire->read();
    uint8_t azlo = this->_SensorWire->read();
    uint8_t mxhi = this->_SensorWire->read();
    uint8_t mxlo = this->_SensorWire->read();
    uint8_t myhi = this->_SensorWire->read();
    uint8_t mylo = this->_SensorWire->read();
    uint8_t mzhi = this->_SensorWire->read();
    uint8_t mzlo = this->_SensorWire->read();


    /* Read and shift values from registers into integers.
    Accelerometer data is 14-bit and left-aligned. Shift two bits right. */
    this->_ax = (int16_t)((axhi << 8) | axlo) >> 2;
    this->_ay = (int16_t)((ayhi << 8) | aylo) >> 2;
    this->_az = (int16_t)((azhi << 8) | azlo) >> 2;

    this->_mx = (int16_t)((mxhi << 8) | mxlo);
    this->_my = (int16_t)((myhi << 8) | mylo);
    this->_mz = (int16_t)((mzhi << 8) | mzlo);

    this->prevMeasMicros = micros();

    // Convert acceleration from int16_t to float in units of [G's]
    switch (this->accelRange)
    {
        case (ACCEL_RNG_2G):
            this->_ax *= ACCEL_MSS_LSB_2G;
            this->_ay *= ACCEL_MSS_LSB_2G;
            this->_az *= ACCEL_MSS_LSB_2G;
            break;
        case (ACCEL_RNG_4G):
            this->_ax *= ACCEL_MSS_LSB_4G;
            this->_ay *= ACCEL_MSS_LSB_4G;
            this->_az *= ACCEL_MSS_LSB_4G;
            break;
        case (ACCEL_RNG_8G):
            this->_ax *= ACCEL_MSS_LSB_8G;
            this->_ay *= ACCEL_MSS_LSB_8G;
            this->_az *= ACCEL_MSS_LSB_8G;
            break;
        default:
            return false;
            break;
    }

    // #ifdef FX0S8700_ACCEL_RANGE_CHECK
    //     if (this->ax > this->accelRangeCheck || this->ax < (-1.0f * this->accelRangeCheck))
    //     {
    //         this->ax = 0.0f;
    //         return -1;
    //     }
    //     else if (this->ay > this->accelRangeCheck || this->ay < (-1.0f * this->accelRangeCheck))
    //     {
    //         this->ay = 0.0f;
    //         return -2;
    //     }
    //     else if (this->az > this->accelRangeCheck || this->az < (-1.0f * this->accelRangeCheck))
    //     {
    //         this->az = 1.0f;
    //         return -3;
    //     }
    // #endif


    // Convert magnetometer to uT
    this->_mx *= MAG_UT_LSB;
    this->_my *= MAG_UT_LSB;
    this->_mz *= MAG_UT_LSB;

    // #ifdef FXOS8700_MAG_RANGE_CHECK
    //     if (this->mx > this->magRangeCheck || this->mx < (-1.0f * this->magRangeCheck))
    //         return -4;
    //     else if ((this->my > this->magRangeCheck || this->my < (-1.0f * this->magRangeCheck)))
    //         return -5;
    //     else if (this->mz > this->magRangeCheck || this->mz < (-1.0f * this->magRangeCheck))
    //         return -6;
    // #endif

    return true;
}


/* Return x-acceleromter measurement in [G's] */
float FXOS8700AccelMag::GetAx()
{
    return this->_ax;
}


/* Return y-acceleromter measurement in [G's] */
float FXOS8700AccelMag::GetAy()
{
    return this->_ay;
}


/* Return z-acceleromter measurement in [G's] */
float FXOS8700AccelMag::GetAz()
{
    return this->_az;
}


/* Return x-magnetometer measurement in [uT] */
float FXOS8700AccelMag::GetMx()
{
    return this->_mx;
}


/* Return y-magnetometer measurement in [uT] */
float FXOS8700AccelMag::GetMy()
{
    return this->_my;
}

/* Return z-magnetometer measurement in [uT] */
float FXOS8700AccelMag::GetMz()
{
    return this->_mz;
}


// ------------------------------------
// Private methods
// ------------------------------------


// ----------------------------------------------------------------------------
// I2Cwrite8(byte regOfInterest, byte valToWrite)
// ----------------------------------------------------------------------------
/**
 * Write to FXOS8700 register over I2C.
 * 
 * @param regOfInterest  Register address on device.
 * @param valToWrite     Value to write to register.
 */
void FXOS8700AccelMag::I2Cwrite8(uint8_t regOfInterest, uint8_t valToWrite)
{
    // Init. communication
    this->_SensorWire->beginTransmission(FXOS8700_ADDRESS);
    this->_SensorWire->write((uint8_t)regOfInterest);
    this->_SensorWire->write((uint8_t)valToWrite);
    this->_SensorWire->endTransmission();
}


// ----------------------------------------------------------------------------
// I2Cread8(byte regOfInterest)
// ----------------------------------------------------------------------------
/**
 * Read FXOS8700 register value over I2C.
 * 
 * @param regOfInterest  Register address on device.
 * @return               Value/data in register.
 */
uint8_t FXOS8700AccelMag::I2Cread8(uint8_t regOfInterest)
{
    uint8_t val;

    // Init. communication
    this->_SensorWire->beginTransmission((uint8_t)FXOS8700_ADDRESS);
    this->_SensorWire->write((uint8_t)regOfInterest);

    // Check for failure
    if (this->_SensorWire->endTransmission(false) != 0) return 0;
    
    // Read register
    this->_SensorWire->requestFrom((uint8_t)FXOS8700_ADDRESS, (uint8_t)1);
    val = this->_SensorWire->read();

    return val;
}
