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
 * GitHub Code Repository: https://github.com/michaelwro/Arduino-LIS3MDL
 * LIS3MDL Datasheet: https://www.st.com/resource/en/datasheet/lis3mdl.pdf
 */

#include <Wire.h>
#include <Arduino.h>
#include "sensors/lis3mdl_magnetometer.h"




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
        #if defined(LIS3MDL_DEBUG)
        DebugPort.println("LIS3MDL:Initialize ERROR: Unfamiliar measurement range specified. Check code.");
        #endif
        return false;
    }

    this->_range = measRange;

    // Read the device ID to ensure we're connected and it matches
    connSensorID = this->I2Cread8(LIS3MDL_WHOAMI);
    if (connSensorID != 0x3D)
    {
        #if defined(LIS3MDL_DEBUG)
        DebugPort.println("LIS3MDL:Initialize ERROR: Sensor ID mismatch. Check connection.");
        #endif
        return false;
    }

    /**
     * Set Control Register 1 params (No temp. sensor):
     * XY AXIS PERFORMANCE MODE AND ODR
     * 
     * 0b (TEMP_EN) (OM1) (OM0) (DO2) (DO1) (DO0) (FAST_ODR) (ST)
     * 
     * 0b01100010  *Ultra-high performance, FAST_ODR=true, 155Hz
     * 0b01000110  High performance mode, FAST_ODR=true, 300Hz
     * 0b01111100  Ultra-high performance, FAST_ODR=false, 80Hz
     */
    uint8_t ctrlReg1Config = 0b01100010;


    /**
     * Set Control Register 2 params:
     * MEASUREMENT RANGE, REBOOT, SOFT RESET
     * Remember to set range in constructor!
     * 
     * 0b (0) (FS1) (FS0) (0) (REBOOT) (SOFT_RS) (0) (0)
     * 
     * 0b00000000  *+/- 4 gauss range
     * 0b00100000  +/- 8 gauss range
     * 0b01000000  +/- 12 gauss range
     * 0b01100000  +/- 16 gauss range
     */
    uint8_t ctrlReg2Config;
    switch (this->_range)
    {
        case LIS3MDL_RANGE_4G:
            ctrlReg2Config = 0b00000000;
            break;
        case LIS3MDL_RANGE_8G:
            ctrlReg2Config = 0b00100000;
            break;
        case LIS3MDL_RANGE_12G:
            ctrlReg2Config = 0b01000000;
            break;
        case LIS3MDL_RANGE_16G:
            ctrlReg2Config = 0b01100000;
            break;
        default:
            ctrlReg2Config = 0b00000000;
            #if defined(LIS3MDL_DEBUG)
            DebugPort.println("LIS3MDL:Initialize WARNING: Invalid measurement range. Check code.");
            #endif
            return false;
            break;
    }

    /**
     * Set Control Register 3 params:
     * SYSTEM OPERATION MODE
     * 
     * 0b (0) (0) (LP) (0) (0) (SIM) (MD1) (MD0)
     * 
     * 0b00000000  LP=0, SIM=0, continuous mode
     */
    uint8_t ctrlReg3Config = 0b00000000;

    /**
     * Set Control Register 4 params:
     * Z AXIS OPERATION MODE, ENDIANESS
     * 
     * 0b (0) (0) (0) (0) (OMZ1) (OMZ0) (BLE) (0)
     * 
     * 0b00001100  *Ultra-high performance mode, lsb at lower address
     * 0b00001000  High performance mode
     */
    uint8_t ctrlReg4Config = 0b00001100;

    /**
     * Set Control Register 5 params
     * 
     * 0b (FAST_READ) (BDU) (0) (0) (0) (0) (0) (0)
     * 
     * 0b00000000  *FAST_READ=false, continuous update
     */
    uint8_t ctrlReg5Config = 0b00000000;

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
            this->_mx = ((float)mxRaw / 6842.0f) * 100.0f;
            this->_my = ((float)myRaw / 6842.0f) * 100.0f;
            this->_mz = ((float)mzRaw / 6842.0f) * 100.0f;
            break;
        case LIS3MDL_RANGE_8G:
            this->_mx = ((float)mxRaw / 3421.0f) * 100.0f;
            this->_my = ((float)myRaw / 3421.0f) * 100.0f;
            this->_mz = ((float)mzRaw / 3421.0f) * 100.0f;
            break;
        case LIS3MDL_RANGE_12G:
            this->_mx = ((float)mxRaw / 2281.0f) * 100.0f;
            this->_my = ((float)myRaw / 2281.0f) * 100.0f;
            this->_mz = ((float)mzRaw / 2281.0f) * 100.0f;
            break;
        case LIS3MDL_RANGE_16G:
            this->_mx = ((float)mxRaw / 1711.0f) * 100.0f;
            this->_my = ((float)myRaw / 1711.0f) * 100.0f;
            this->_mz = ((float)mzRaw / 1711.0f) * 100.0f;
            break;
        default:
            #if defined(LIS3MDL_DEBUG)
            DebugPort.println("LIS3MDL:ReadSensor ERROR: Invalid range specified. Check code.");
            #endif
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
