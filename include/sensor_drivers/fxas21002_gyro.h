// ----------------------------------------------------------------------------
// FXAS21002C GYROSCOPE SENSOR LIBRARY
// A custom sensor library for the FXAS21002C I2C gyroscope sensor. Inspired by
// Adafruit's FXAS21002C Library (see Resources).
// 
// Code By: Michael Wrona | B.S. Aerospace Engineering
// Created: 25 July 2020
// ----------------------------------------------------------------------------
/**
 * This is the sensor library for the FXAS21002C 3-axis gyroscope sensor. This
 * library was inspired by Adafruit's FXAS21002C Library (see Resources). Tested
 * and verified with Adafruit's FXAS21002C/FXOS8700 9-DOF IMU and an Arduino Uno.
 * 
 * Resources
 * ---------
 * ~ Adafruit FXAS21002C Sensor Library (GitHub):
 *     https://github.com/adafruit/Adafruit_FXAS21002C
 * ~ FXOX8700 + FXAS21002 9-DOF IMU (Adafruit):
 *     https://www.adafruit.com/product/3463
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <limits.h>
#include "constants.h"
#include "conversions.h"
#include "hummingbird_config.h"


// ----------------------------------------------------------------------------
// Device I2C addresses
// ----------------------------------------------------------------------------
constexpr uint8_t FXAS21002C_ADDRESS  = 0x21;  // 7-bit address
constexpr uint8_t FXAS21002C_ID       = 0xD7;  // Device ID

// ----------------------------------------------------------------------------
// Gyro measurement sensitivity settings
// ----------------------------------------------------------------------------
//typedef enum
//{
//    GYRO_SENS_250  = 0.0078125f, // 250dps sensitivity
//    GYRO_SENS_500  = 0.015625f,  // 500dps sensitivity
//    GYRO_SENS_1000 = 0.03125f,  // 1000dps sensitivity
//    GYRO_SENS_2000 = 0.0625f  // 2000dps sensitivity
//} GyroSensitivity_t;

constexpr float GYRO_SENS_250   = 0.0078125f; // 250dps sensitivity
constexpr float GYRO_SENS_500   = 0.015625f;  // 500dps sensitivity
constexpr float GYRO_SENS_1000  = 0.03125f;  // 1000dps sensitivity
constexpr float GYRO_SENS_2000  = 0.0625f;  // 2000dps sensitivity

// ----------------------------------------------------------------------------
// Device registers
// ----------------------------------------------------------------------------
typedef enum
{
    GYRO_REG_STATUS   = 0x00,  //GYRO_REGISTER_STATUS 
    GYRO_REG_XOUT_MSB = 0x01,  // GYRO_REGISTER_OUT_X_MSB
    GYRO_REG_XOUT_LSB = 0x02,
    GYRO_REG_YOUT_MSB = 0x03,
    GYRO_REG_YOUT_LSB = 0x04,
    GYRO_REG_ZOUT_MSB = 0x05,
    GYRO_REG_ZOUT_LSB = 0x06,
    GYRO_REG_ID       = 0x0C,  // GYRO_REGISTER_WHO_AM_I
    GYRO_REG_CTRL0    = 0x0D,  // GYRO_REGISTER_CTRL_REG0
    GYRO_REG_CTRL1    = 0x13,
    GYRO_REG_CTRL2    = 0x14

} GyroRegisters_t;

// ----------------------------------------------------------------------------
// Gyro measurement range
// ----------------------------------------------------------------------------
typedef enum
{
    GYRO_RNG_250DPS = 250,  // 250dps range
    GYRO_RNG_500DPS = 500,  // 500dps range
    GYRO_RNG_1000DPS = 1000,  // 1000dps range
    GYRO_RNG_2000DPS = 2000  // 2000dps range
} GyroRanges_t;

// ----------------------------------------------------------------------------
// Gyro sensor class
// ----------------------------------------------------------------------------
class FXAS21002Gyro
{
public:
    FXAS21002Gyro(TwoWire *wireInput = &SENSOR_I2C);
//        ~FXAS21002Gyro();
    bool Initialize(GyroRanges_t rng = GYRO_RNG_1000DPS);
    bool ReadSensor();
    float GetGx();
    float GetGy();
    float GetGz();  
    uint32_t prevMeasMicros;  // [us] Previous measurement micros()


protected:
private:
    void I2Cwrite8(uint8_t regOfInterest, uint8_t valToWrite);
    uint8_t I2Cread8(uint8_t regOfInterest);
    float _gx;  // Gyro x reading, [rad/s]
    float _gy;  // Gyro y reading, [rad/s]
    float _gz;  // Gyro z reading, [rad/s]
    GyroRanges_t gyroRange;
    TwoWire *_SensorWire;
};
