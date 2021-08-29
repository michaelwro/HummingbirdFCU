// ----------------------------------------------------------------------------
// FXAS21002C GYROSCOPE SENSOR I2C DRIVER CODE
// 
// Code By: Michael Wrona
// Created: 25 July 2020
// Modified: 26 Aug 2021
// ----------------------------------------------------------------------------
// This is the sensor library for the FXAS21002C 3-axis gyroscope sensor in 
// I2C mode.
// Datasheet Specs:
//   16-bit ADC
//   +/- 250dps to 2000dps measurement ranges
//   Integrated LPF
//   8-bit temperature sensor
//   Temperature sensitivity of +/- 0.08 %/degC (max)
//   Nonlinearity of +/- 0.5 %FSR
//   0.025 dps/sqrt(Hz) noise density (FS=200Hz, CTRL_REG0[FS] = 00, CTRL_REG0[BW] = 00)


#pragma once

#include <Arduino.h>
#include <Wire.h>
// #include <limits.h>
#include "constants.h"
#include "debugging.h"
#include "conversions.h"
#include "hummingbird_config.h"


#ifdef DEBUG
#define FXAS21002_DEBUG  // Toggle FXAS21002 gyro debug messages
#endif



// ----------------------------------------------------------------------------
// Device I2C addresses
// ----------------------------------------------------------------------------
constexpr uint8_t FXAS21002C_ADDRESS  = 0x21;  // 7-bit address
constexpr uint8_t FXAS21002C_ID       = 0xD7;  // Device ID


/**
 * Gyro measurement sensitivities.
 * Conversion factors to convert between LSB and deg/sec.
 */
constexpr float GYRO_SENS_250   = 0.0078125f; // 250dps sensitivity [dps/LSB]
constexpr float GYRO_SENS_500   = 0.015625f;  // 500dps sensitivity [dps/LSB]
constexpr float GYRO_SENS_1000  = 0.03125f;  // 1000dps sensitivity [dps/LSB]
constexpr float GYRO_SENS_2000  = 0.0625f;  // 2000dps sensitivity [dps/LSB]


/**
 * FXAS21002 device registers
 */
typedef enum
{
    GYRO_REG_STATUS   = 0x00,
    GYRO_REG_XOUT_MSB = 0x01,
    GYRO_REG_XOUT_LSB = 0x02,
    GYRO_REG_YOUT_MSB = 0x03,
    GYRO_REG_YOUT_LSB = 0x04,
    GYRO_REG_ZOUT_MSB = 0x05,
    GYRO_REG_ZOUT_LSB = 0x06,
    GYRO_REG_ID       = 0x0C,
    GYRO_REG_TEMP     = 0x12,  //  (p.45)
    GYRO_REG_CTRL0    = 0x0D,  // (p.38)
    GYRO_REG_CTRL1    = 0x13,
    GYRO_REG_CTRL2    = 0x14
} GyroRegisters_t;


/**
 * Gyro measurement ranges.
 */
typedef enum
{
    GYRO_RNG_250DPS = 250,  // 250dps range
    GYRO_RNG_500DPS = 500,  // 500dps range
    GYRO_RNG_1000DPS = 1000,  // 1000dps range
    GYRO_RNG_2000DPS = 2000  // 2000dps range
} GyroRanges_t;


/**
 * NXP FXAS21002 gyro sensor driver.
 */
class FXAS21002Gyro
{
public:
    FXAS21002Gyro(TwoWire *wireInput = &SENSOR_I2C);
    ~FXAS21002Gyro() {};
    bool Initialize(GyroRanges_t rng = GYRO_RNG_1000DPS);
    bool ReadSensor();
    float GetTemperature();
    float GetGx();
    float GetGy();
    float GetGz();  
    uint32_t prevMeasMicros;  ///< Previous measurement micros()
protected:
private:
    void I2Cwrite8(uint8_t regOfInterest, uint8_t valToWrite);
    uint8_t I2Cread8(uint8_t regOfInterest);
    float _gx;  ///< Gyro x reading, [deg/s]
    float _gy;  ///< Gyro y reading, [deg/s]
    float _gz;  ///< Gyro z reading, [deg/s]
    GyroRanges_t gyroRange;  ///< Selected gyro measurement range.
    TwoWire *_SensorWire;  ///< I2C bus the sensor is connected to.
};
