// ----------------------------------------------------------------------------
// FXOS8700 ACCELEROMETER & MAGNETOMETER SENSOR LIBRARY
// 
// Code By: Michael Wrona
// Created: 31 Jan 2021
// ----------------------------------------------------------------------------
/**
 * Source code for the FXOS8700 accelerometer and magnetometer sensor library.
 */

#pragma once

#include <Wire.h>
#include <Arduino.h>
#include "hummingbird_config.h"
#include "constants.h"

// #define FX0S8700_ACCEL_RANGE_CHECK  // Enable if range checks for the accelerometer measurements should be performed
// #define FXOS8700_MAG_RANGE_CHECK  // Enable if range checks for the magnetometer measurements should be performed

constexpr uint8_t FXOS8700_ID       = 0xC7;
constexpr uint8_t FXOS8700_ADDRESS  = 0x1F;

// ----------------------------------------------------------------------------
// Define conversion factors to convert accel. and mag. data to floats
// ----------------------------------------------------------------------------
constexpr float ACCEL_MSS_LSB_2G    = 0.000244f;    // Convert to [m/s/s]
constexpr float ACCEL_MSS_LSB_4G    = 0.000488f;    // Convert to [m/s/s]
constexpr float ACCEL_MSS_LSB_8G    = 0.000976f;    // Convert to [m/s/s]
constexpr float MAG_UT_LSB          = 0.1f;         // Convert to [uT]

// ----------------------------------------------------------------------------
// Accelerometer ranges
// ----------------------------------------------------------------------------
typedef enum
{
    ACCEL_RNG_2G = 0x00,
    ACCEL_RNG_4G = 0x01,
    ACCEL_RNG_8G = 0x02
} AccelRanges_t;

// ----------------------------------------------------------------------------
// Accelerometer & magnetometer registers
// ----------------------------------------------------------------------------
typedef enum
{
    ACCELMAG_REG_STATUS     = 0x00,          
    ACCELMAG_REG_ID         = 0x0D,       
    ACCELMAG_REG_XYZ_CFG    = 0x0E,
    ACCELMAG_REG_AOUT_X_MSB = 0x01,
    ACCELMAG_REG_AOUT_X_LSB = 0x02,
    ACCELMAG_REG_AOUT_Y_MSB = 0x03,
    ACCELMAG_REG_AOUT_Y_LSB = 0x04,
    ACCELMAG_REG_AOUT_Z_MSB = 0x05,
    ACCELMAG_REG_AOUT_Z_LSB = 0x06,
    ACCELMAG_REG_CTRL1      = 0x2A,       
    ACCELMAG_REG_CTRL2      = 0x2B,
    ACCELMAG_REG_CTRL3      = 0x2C,
    ACCELMAG_REG_CTRL4      = 0x2D,
    ACCELMAG_REG_CTRL5      = 0x2E,
    ACCELMAG_REG_MSTATUS    = 0x32,         
    ACCELMAG_REG_MOUT_X_MSB = 0x33,      
    ACCELMAG_REG_MOUT_X_LSB = 0x34,
    ACCELMAG_REG_MOUT_Y_MSB = 0x35,
    ACCELMAG_REG_MOUT_Y_LSB = 0x36,
    ACCELMAG_REG_MOUT_Z_MSB = 0x37,
    ACCELMAG_REG_MOUT_Z_LSB = 0x38,
    ACCELMAG_REG_MCTRL1     = 0x5B,      
    ACCELMAG_REG_MCTRL2     = 0x5C,
    ACCELMAG_REG_MCTRL3     = 0x5D
} MagAccelRegisters_t;


class FXOS8700AccelMag
{
public:
    FXOS8700AccelMag(TwoWire *wireInput = &SensorI2C);
    bool Initialize(AccelRanges_t accRange = ACCEL_RNG_4G);
    bool ReadSensor();
    uint32_t prevMeasMicros;  // [us] Previous measurement micros()
    float GetAx();
    float GetAy();
    float GetAz();
    float GetMx();
    float GetMy();
    float GetMz();

    // float accelRangeCheck;  // For checking the measrement range. Should fall within configured range.
    // float magRangeCheck;
    AccelRanges_t accelRange;
protected:
private:
    float _ax;  // X-acceleration [G's]
    float _ay;  // Y-acceleration [G's]
    float _az;  // Z-acceleration [G's]
    float _mx;  // X-magnetometer reading [uT]
    float _my;  // Y-magnetometer reading [uT]
    float _mz;  // Z-magnetometer reading [uT]
    TwoWire *_SensorWire;  // I2C bus that the sensor is on
    uint8_t I2Cread8(uint8_t regOfInterest);
    void I2Cwrite8(uint8_t regOfInterest, uint8_t valToWrite);

};
