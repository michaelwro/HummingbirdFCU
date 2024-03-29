// ----------------------------------------------------------------------------
// FXOS8700 ACCELEROMETER & MAGNETOMETER SENSOR LIBRARY
// 
// Code By: Michael Wrona
// Created: 26 Aug 2021
// ----------------------------------------------------------------------------
// Source code for the FXOS8700 accelerometer and magnetometer sensor library.
// IMPORTANT NOTES:
// - Hybrid mode is needed because the magnetometer and accelerometer USE THE 
// SAME ADC! See p.22 for more.
// - Because we are using the LIS3MDL compass in the GPS, I chose to disable 
// hybrid mode and only use the FXOS8700's accelerometer (8/26/2021).



#include <Arduino.h>
#include <Wire.h>
#include "sensor_drivers/fxos8700_accelmag.h"



/**
 * Constructor for the FXOS8700 Accelerometer/Magnetometer class.
 * 
 * @param wireInput I2C bus/wire interface that the device is connected to
 */
FXOS8700AccelMag::FXOS8700AccelMag(TwoWire *wireInput)
{
    this->_ax = 0.0f;  // Zero out variables
    this->_ay = 0.0f;
    this->_az = 0.0f;
    this->prevMeasMicros = micros();
    this->_SensorWire = wireInput;
}


/**
 * Initialize accelerometer, set accel. measurement range, configure magnetometer.
 * 
 * @param accRange  Desired accelerometer measurement range.
 * @see AccelRanges_t
 */
bool FXOS8700AccelMag::Initialize(AccelRanges_t accRange)
{
    uint8_t connectedSensorID;

    this->_SensorWire->begin();  // Init. communication
    this->accelRange = accRange; // Set accelerometer range
    
    // Check to make sure the ID register on the sensor matches the expected FXOS8700 ID.
    connectedSensorID = this->I2Cread8(ACCELMAG_REG_ID);
    
    if (connectedSensorID != FXOS8700_ID)
    {
        #ifdef FXOS8700_DEBUG
        DEBUG_PRINTLN("FXOS8700ACCELMAG::Initialize ERROR: Received sensor ID does not match expected.");
        #endif
        return false;
    }
    
    // Set sensor to STANDBY MODE in order to make changes to/configure a few registers.
    this->I2Cwrite8(ACCELMAG_REG_CTRL1, 0);
    
    // Begin configuration
    switch (this->accelRange)  // Set accel. measurement range
    {
        case (ACCEL_RNG_2G):
            this->I2Cwrite8(ACCELMAG_REG_XYZ_CFG, 0x00);
            break;
        case (ACCEL_RNG_4G):
            this->I2Cwrite8(ACCELMAG_REG_XYZ_CFG, 0x01);
            break;
        case (ACCEL_RNG_8G):
            this->I2Cwrite8(ACCELMAG_REG_XYZ_CFG, 0x02);
            break;
        default:
            #ifdef FXOS8700_DEBUG
            DEBUG_PRINTLN("FXOS8700ACCELMAG::Initialize ERROR: Unknown accelerometer range specified.");
            #endif
            return false;
            break;
    }

    // Configure accel.
    this->I2Cwrite8(ACCELMAG_REG_CTRL2, 0x12);  // Self-test disabled. Reset disabled. Sleep mode OSR mode = hi. rez. Wake mode OSR mode = hi. rez.

    // Low-noise mode only works for +/- 2g and 4g modes. Change bit values according to the measurement range
    if (this->accelRange == ACCEL_RNG_8G)
    {
        // normal mode instead of low-noise
        // Sleep mode ODR = 50Hz. Sensor ODR = 400Hz single sensor = 200Hz hybrid mode. Full-scale range mode = normal mode. Fast read mode = normal. Active mode.
        this->I2Cwrite8(ACCELMAG_REG_CTRL1, 0x09);
    }
    else
    {
        // Sleep mode ODR = 50Hz. Sensor ODR = 400Hz single sensor = 200Hz hybrid mode. Full-scale range mode = low-noise mode. Fast read mode = normal. Active mode.
        this->I2Cwrite8(ACCELMAG_REG_CTRL1, 0x0D);
    }

    // Configure magnetometer and disable hybrid mode (accel. only)
    this->I2Cwrite8(ACCELMAG_REG_MCTRL1, 0x10);  // Auto-calib. disabled. m_rst = 0. m_ost = 0. Mag. oversampling = 0b100. Accelerometer active, mag. disabled (no hybrid mode).
    this->I2Cwrite8(ACCELMAG_REG_MCTRL2, 0x30);  // Auto-jump to 0x33 (see p.98). Disable mag. min/max detection. (0x20 is known to work fine if this doesn't work properly).
    delay(100);  // Slight delay after configuring. P.45 says to wait at least 1ms after issuing a reset command.

    return true;
}


/**
 * Read acceleration data from the FXOS8700 sensor.
 * 
 * @return  True if successful
 */
bool FXOS8700AccelMag::ReadSensor()
{
    // Raw measurements
    int16_t axRaw = 0;
    int16_t ayRaw = 0;
    int16_t azRaw = 0;
    int16_t mxRaw = 0;
    int16_t myRaw = 0;
    int16_t mzRaw = 0;

    // Read 13 bytes from sensor
    this->_SensorWire->beginTransmission((uint8_t)FXOS8700_ADDRESS);
    this->_SensorWire->write(ACCELMAG_REG_STATUS | 0x80);
    this->_SensorWire->endTransmission();
    this->_SensorWire->requestFrom((uint8_t)FXOS8700_ADDRESS, (uint8_t)13);  // status plus 6 channels = 13 bytes

    // Read the registers
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


    /**
     * Read and shift values from registers into integers.
     * Accelerometer data is 14-bit and left-aligned. Shift two bits right.
     * See p.28 for datasheet's code example.
     */
    axRaw = (int16_t)((axhi << 8) | axlo) >> 2;
    ayRaw = (int16_t)((ayhi << 8) | aylo) >> 2;
    azRaw = (int16_t)((azhi << 8) | azlo) >> 2;

    // mxRaw = (int16_t)((mxhi << 8) | mxlo);
    // myRaw = (int16_t)((myhi << 8) | mylo);
    // mzRaw = (int16_t)((mzhi << 8) | mzlo);

    this->prevMeasMicros = micros();

    // Convert acceleration from int16_t to float in units of [g's]
    switch (this->accelRange)
    {
        case (ACCEL_RNG_2G):
            this->_ax = (float)axRaw * ACCELMAG_CVT_GS_2G;
            this->_ay = (float)ayRaw * ACCELMAG_CVT_GS_2G;
            this->_az = (float)azRaw * ACCELMAG_CVT_GS_2G;
            break;
        case (ACCEL_RNG_4G):
            this->_ax = (float)axRaw * ACCELMAG_CVT_GS_4G;
            this->_ay = (float)ayRaw * ACCELMAG_CVT_GS_4G;
            this->_az = (float)azRaw * ACCELMAG_CVT_GS_4G;
            break;
        case (ACCEL_RNG_8G):
            this->_ax = (float)axRaw * ACCELMAG_CVT_GS_8G;
            this->_ay = (float)ayRaw * ACCELMAG_CVT_GS_8G;
            this->_az = (float)azRaw * ACCELMAG_CVT_GS_8G;
            break;
        default:
            return false;  // return false if not recognized
            break;
    }

    return true;
}


/**
 * Return x-acceleromter measurement in [G's]
 */
float FXOS8700AccelMag::GetAx()
{
    return this->_ax;
}


/**
 * Return y-acceleromter measurement in [G's]
 */
float FXOS8700AccelMag::GetAy()
{
    return this->_ay;
}


/**Return z-acceleromter measurement in [G's]
 */
float FXOS8700AccelMag::GetAz()
{
    return this->_az;
}


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


/**
 * Read FXOS8700 register value over I2C.
 * 
 * @param regOfInterest Register address on device.
 * @return Value/data in register.
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
