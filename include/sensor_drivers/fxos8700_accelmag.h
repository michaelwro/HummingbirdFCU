// ----------------------------------------------------------------------------
// FXOS8700 ACCELEROMETER & MAGNETOMETER SENSOR LIBRARY
// 
// Code By: Michael Wrona
// Created: 26 Aug 2021
// ----------------------------------------------------------------------------
/**
 * Source code for the FXOS8700 accelerometer and magnetometer sensor library.
 * 
 * IMPORTANT NOTES
 * - Hybrid mode is needed because the magnetometer and accelerometer USE THE 
 * SAME ADC! See p.22 for more.
 * - Because we are using the LIS3MDL compass in the GPS, I chose to disable 
 * hybrid mode and only use the FXOS8700's accelerometer (8/26/2021).
 * 
 * Accelerometer Datsheet Specs
 * ----------------------------
 * ~ 14-bit ADC
 * ~ +/- 2g to 8g accel. range
 * ~ Temperature sensitivity: +/- 0.01 %/degC
 * ~ Nonlinearity: +/- 0.5 %FSR
 * ~ 126 ug/sqrt(Hz) noise density
 * ~ Up to 800Hz FS in single sensor mode
 * ~ Up to 400Hz FS in hybrid mode
 * 
 * Magnetometer Datsheet Specs
 * ----------------------------
 * ~ 16-bit magnetometer ADC
 * ~ +/- 1200uT mag. range
 * ~ Temperature sensitivity: +/- 0.1 %/degC
 * ~ Nonlinearity: +/- 1 %FSR
 * ~ 1.5uT RMS noise (max)
 * ~ Up to 800Hz FS in single sensor mode
 * ~ Up to 400Hz FS in hybrid mode
 */


#pragma once

#include <Wire.h>
#include <Arduino.h>
#include "hummingbird_config.h"
#include "constants.h"

constexpr uint8_t FXOS8700_ID       = 0xC7;  // Unique ID
constexpr uint8_t FXOS8700_ADDRESS  = 0x1F;  // I2C address

// ----------------------------------------------------------------------------
// Define conversion factors to convert accel. and mag. data to floats
// ----------------------------------------------------------------------------
constexpr float ACCELMAG_CVT_GS_2G    = 0.000244140625f;   // Convert int16 to g's for +/- 2G range
constexpr float ACCELMAG_CVT_GS_4G    = 0.00048828125f;    // Convert int16 to g's for +/- 4G range
constexpr float ACCELMAG_CVT_GS_8G    = 0.0009765625f;     // Convert int16 to g's for +/- 8G range
constexpr float ACCELMAG_CVT_UT       = 0.1f;         // Convert int16 to uT

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
    FXOS8700AccelMag(TwoWire *wireInput = &SENSOR_I2C);
    bool Initialize(AccelRanges_t accRange = ACCEL_RNG_4G);
    bool ReadSensor();
    uint32_t prevMeasMicros;  // [us] Previous measurement micros()
    float GetAx();
    float GetAy();
    float GetAz();
    AccelRanges_t accelRange;
protected:
private:
    float _ax;  // X-acceleration [G's]
    float _ay;  // Y-acceleration [G's]
    float _az;  // Z-acceleration [G's]
    TwoWire *_SensorWire;  // I2C bus that the sensor is on
    uint8_t I2Cread8(uint8_t regOfInterest);
    void I2Cwrite8(uint8_t regOfInterest, uint8_t valToWrite);

};
