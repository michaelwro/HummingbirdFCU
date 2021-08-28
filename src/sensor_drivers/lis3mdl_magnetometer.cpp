/**
 * LIS3MDL 3-AXIS MAGNETOMETER SENSOR LIBRARY
 * 
 * Used to communicate with the STMicroelectronic's LIS3MDL magnetometer
 * sensor over I2C.
 * 
 * Code By: Michael Wrona
 * Created: 17 Jan 2021
 * 
 * Resources:
 * LIS3MDL Datasheet: https://www.st.com/resource/en/datasheet/lis3mdl.pdf
 * 
 * Datasheet Specs
 * ---------------
 * ~ +/- 4 Gauss to 16 Gauss measurement range
 * ~ 4.1 mgauss RMS noise (max)
 * ~ Nonlinearity: +/- 0.12 %FSR
 */

#include <Wire.h>
#include <Arduino.h>
#include "sensor_drivers/lis3mdl_magnetometer.h"




// ----------------------------------------------------------------------------
// LIS3MDL_Mag(TwoWire *userWire)
// ----------------------------------------------------------------------------
/**
 * @brief I2C Sensor class for the LIS3MDL magnetometer.
 * 
 * @param userthis->_SensorWire-> I2C bus that the sensor is attached to. Default this->_SensorWire->
 */
LIS3MDL_Mag::LIS3MDL_Mag(TwoWire *userWire)
{
    this->_SensorWire = userWire;
    this->_mx = 0.0f;
    this->_my = 0.0f;
    this->_mz = 0.0f;
    this->prevMeasMicros = micros();
}



// ----------------------------------------------------------------------------
// LIS3MDL_Mag()
// ----------------------------------------------------------------------------
/**
 * Inityialize the LIS3MDL magnetometer and specify the measurement range.
 * 
 * @param measRange     Magnetometer measurement range Choose between 
 *                      LIS3MDL_RANGE_4G, LIS3MDL_RANGE_8G,
 *                      LIS3MDL_RANGE_12G, LIS3MDL_RANGE_16G.
 */
bool LIS3MDL_Mag::Initialize(LIS3MDL_MeasRange_t measRange)
{
    uint8_t connSensorID;  // Check that the connected sensor ID matches the expected one

    this->_SensorWire->begin();

    if (measRange > LIS3MDL_RANGE_16G)
    {
        #ifdef LIS3MDL_DEBUG
        DEBUG_PRINTLN("LIS3MDL::Initialize ERROR: Unknown measurement range specified.");
        #endif
        return false;
    }

    this->_range = measRange;

    // Read the device ID to ensure we're connected and it matches
    connSensorID = this->I2Cread8(LIS3MDL_WHOAMI);
    if (connSensorID != 0x3D)
    {
        #ifdef LIS3MDL_DEBUG
        DEBUG_PRINTLN("LIS3MDL::Initialize ERROR: Sensor ID mismatch. Check connection.");
        #endif
        return false;
    }

    /* Set Control Register 1 params */
    // uint8_t ctrlReg1Config = 0b01100010;
    // Enable temp. sensor. XY operative mode = ultra-high performance. ODR = 0b100 (10Hz). Enable fast ODR. Disable self-test.
    // With fast-ODR mode enabled and ultra-high performance mode selected, this yields a final ODR of 155Hz.
    uint8_t ctrlReg1Config = 0xF2;  // 0b11110010


    /* Set Control Register 2 params */
    // This sets the data range
    uint8_t ctrlReg2Config;
    switch (this->_range)
    {
        case LIS3MDL_RANGE_4G:
            ctrlReg2Config = 0x00;  // 0b00000000
            break;
        case LIS3MDL_RANGE_8G:
            ctrlReg2Config = 0x20;  // 0b00100000
            break;
        case LIS3MDL_RANGE_12G:
            ctrlReg2Config = 0x40;  // 0b01000000
            break;
        case LIS3MDL_RANGE_16G:
            ctrlReg2Config = 0x60;  // 0b01100000
            break;
        default:
            ctrlReg2Config = 0x00;
            #ifdef LIS3MDL_DEBUG
            DEBUG_PRINTLN("LIS3MDL::Initialize ERROR: Unknown measurement range specified.");
            #endif
            return false;
            break;
    }

    /* Set Control Register 3 params */
    // Low-power disable. 4-wire SPI interface. Continuous conversion mode.
    uint8_t ctrlReg3Config = 0x00;  // 0b00000000

    /* Set Control Register 4 params */
    // Ultra-high performance mode for Z-axis. LSb at lower address.
    uint8_t ctrlReg4Config = 0x0C;  // 0b00001100

    /* Set Control Register 5 params */
    // Disable fast-read. Continuous update.
    uint8_t ctrlReg5Config = 0x00;  // 0b00000000


    // Send config commands
    this->I2Cwrite8(LIS3MDL_CTRL_REG1, ctrlReg1Config);
    this->I2Cwrite8(LIS3MDL_CTRL_REG2, ctrlReg2Config);
    this->I2Cwrite8(LIS3MDL_CTRL_REG3, ctrlReg3Config);
    this->I2Cwrite8(LIS3MDL_CTRL_REG4, ctrlReg4Config);
    this->I2Cwrite8(LIS3MDL_CTRL_REG5, ctrlReg5Config);

    return true;
}


// ----------------------------------------------------------------------------
// ReadSensor()
// ----------------------------------------------------------------------------
/**
 * @brief Read magnetometer registers and extract measurements. Converts raw 
 * readings in Gauss [G] to microtesla [uT].
 * 
 * @returns true if successful, false if invalid measurement range.
 */
bool LIS3MDL_Mag::ReadSensor()
{
    uint8_t xlo, xhi;
    uint8_t ylo, yhi;
    uint8_t zlo, zhi;
    int16_t mxRaw, myRaw, mzRaw;

    // Read the 6 data bytes from sensor
    this->_SensorWire->beginTransmission((uint8_t)LIS3MDL_ADDR);
    this->_SensorWire->write(LIS3MDL_OUT_X_L | 0x80);
    this->_SensorWire->endTransmission();

    // Request 6 register values
    this->_SensorWire->requestFrom((uint8_t)LIS3MDL_ADDR, (uint8_t)6);

    xlo = this->_SensorWire->read();
    xhi = this->_SensorWire->read();
    ylo = this->_SensorWire->read();
    yhi = this->_SensorWire->read();
    zlo = this->_SensorWire->read();
    zhi = this->_SensorWire->read();

    // Bitwise operators to convert to actual readings
    mxRaw = (int16_t)((xhi << 8) | xlo);
    myRaw = (int16_t)((yhi << 8) | ylo);
    mzRaw = (int16_t)((zhi << 8) | zlo);

    // Convert to float and units of micro tesla [uT]. Raw meas. are in Gauss.
    // LSB/gauss values are from LIS3MDL datasheet.
    // 1G = 100uT
    switch(this->_range)
    {
        case LIS3MDL_RANGE_4G:
            this->_mx = (float)mxRaw * 0.01461560947091493715287927506577f;  // 100/6842
            this->_my = (float)myRaw * 0.01461560947091493715287927506577f;
            this->_mz = (float)mzRaw * 0.01461560947091493715287927506577f;
            break;
        case LIS3MDL_RANGE_8G:
            this->_mx = (float)mxRaw * 0.02923121894182987430575855013154f;  // 100/3421
            this->_my = (float)myRaw * 0.02923121894182987430575855013154f;
            this->_mz = (float)mzRaw * 0.02923121894182987430575855013154f;
            break;
        case LIS3MDL_RANGE_12G:
            this->_mx = (float)mxRaw * 0.04384042086804033318719859710653f;  // 100/2281
            this->_my = (float)myRaw * 0.04384042086804033318719859710653f;
            this->_mz = (float)mzRaw * 0.04384042086804033318719859710653f;
            break;
        case LIS3MDL_RANGE_16G:
            this->_mx = (float)mxRaw * 0.05844535359438924605493863237873f;  // 100/1711
            this->_my = (float)myRaw * 0.05844535359438924605493863237873f;
            this->_mz = (float)mzRaw * 0.05844535359438924605493863237873f;
            break;
        default:
            return false;
            break;
    }

    return true;
}


/* Return X-magnetometer reading in [uT] */
float LIS3MDL_Mag::GetMx()
{
    return this->_mx;
}


/* Return Y-magnetometer reading in [uT] */
float LIS3MDL_Mag::GetMy()
{
    return this->_my;
}

/* Return Z-magnetometer reading in [uT] */
float LIS3MDL_Mag::GetMz()
{
    return this->_mz;
}


// ----------------------------------------------------------------------------
// GetTemperature()
// ----------------------------------------------------------------------------
/**
 * Read temperature from the magnetometer sensor and return as float in units 
 * of [C]. Temperature ranges from -40C to +85C. ODR is the same as the mag's ODR.
 * 
 * @returns Floating-point temperature in [C].
 */
float LIS3MDL_Mag::GetTemperature()
{
    uint8_t tlo;
    uint8_t thi;
    int16_t tempRaw;
    float tempC;

    // Request over i2c bus
    this->_SensorWire->beginTransmission((uint8_t)LIS3MDL_ADDR);
    this->_SensorWire->write(LIS3MDL_OUT_TEMP_L);
    this->_SensorWire->endTransmission();

    // Read bytes
    this->_SensorWire->requestFrom((uint8_t)LIS3MDL_ADDR, (uint8_t)2);
    tlo = this->_SensorWire->read();
    thi = this->_SensorWire->read();

    // Convert to float
    // https://electronics.stackexchange.com/questions/292863/magnetometer-lis3mdl-temperature-reading-changes-with-orientation
    tempRaw = (int16_t)((tlo << 8) | thi);  // temperature as a signed int
    tempC =  0.125f * ((float)tempRaw / 256.0f) + 25.0f;  // Temperature scaling: 8 degC/LSB (see p.9). Offset from application note
    return tempC;
}


// ------------------------------------
// PRIVATE METHODS
// ------------------------------------

// ----------------------------------------------------------------------------
// I2Cwrite8(byte regOfInterest, byte valToWrite)
// ----------------------------------------------------------------------------
/**
 * Write to device register over I2C.
 * 
 * @param regOfInterest  Register address on device.
 * @param valToWrite     Value to write to register.
 */
void LIS3MDL_Mag::I2Cwrite8(uint8_t regOfInterest, uint8_t valToWrite)
{
    // Init. communication
    this->_SensorWire->beginTransmission(LIS3MDL_ADDR);
    this->_SensorWire->write((uint8_t)regOfInterest);
    this->_SensorWire->write((uint8_t)valToWrite);

    this->_SensorWire->endTransmission();
}


// ----------------------------------------------------------------------------
// I2Cread8(byte regOfInterest)
// ----------------------------------------------------------------------------
/**
 * Read register value from I2C device.
 * 
 * @param regOfInterest  Register address on device.
 * @return               Value/data in register.
 */
uint8_t LIS3MDL_Mag::I2Cread8(uint8_t regOfInterest)
{
    uint8_t val;

    // Init. communication
    this->_SensorWire->beginTransmission((uint8_t)LIS3MDL_ADDR);
    this->_SensorWire->write((uint8_t)regOfInterest);

    // Check for failure
    if (this->_SensorWire->endTransmission(false) != 0)
        return 0;
    
    // Read register
    this->_SensorWire->requestFrom((uint8_t)LIS3MDL_ADDR, (uint8_t)1);
    val = this->_SensorWire->read();

    this->_SensorWire->endTransmission();

    return val;
}
