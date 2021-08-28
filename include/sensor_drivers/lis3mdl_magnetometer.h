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

#pragma once


#include <Arduino.h>
#include <Wire.h>
#include "debugging.h"
#include "hummingbird_config.h"


#ifdef DEBUG
#define LIS3MDL_DEBUG  // For printing magnetometer debug messages
#endif


// ------------------------------------
// Control Registers
// ------------------------------------

// Default I2C default slave address (0x1C). SDO/SA1 pin connected to GND.
// If you have an issue connecting to the sensor, try changing
// LIS3MDL_ADDR to 0x1C
#define LIS3MDL_ADDR 0x1E
#define LIS3MDL_WHOAMI 0x0F  // ID byte
#define LIS3MDL_CTRL_REG1 0x20  // Control register 1
#define LIS3MDL_CTRL_REG2 0x21  // Control register 2
#define LIS3MDL_CTRL_REG3 0x22  // Control register 3
#define LIS3MDL_CTRL_REG4 0x23  // Control register 4
#define LIS3MDL_CTRL_REG5 0x24  // Control register 5


// ------------------------------------
// Data Registers
// ------------------------------------
typedef enum {
    LIS3MDL_OUT_X_L = 0x28,
    LIS3MDL_OUT_X_H = 0x29,
    LIS3MDL_OUT_Y_L = 0x2A,
    LIS3MDL_OUT_Y_H = 0X2B,
    LIS3MDL_OUT_Z_L = 0x2C,
    LIS3MDL_OUT_Z_H = 0x2D,
    LIS3MDL_OUT_TEMP_L = 0x2E,  // Low-byte for temperature sensor. Stores as twos-compliment.
    LIS3MDL_OUT_TEMP_H = 0x2F  // High-byte for temperature sensor. Stores as twos-compliment.
} LIS3MDL_DataReg_t;


// ------------------------------------
// Data Ranges
// ------------------------------------
typedef enum {
    LIS3MDL_RANGE_4G = 4,  // +/- 4 G range
    LIS3MDL_RANGE_8G = 8,  // +/- 8 G range
    LIS3MDL_RANGE_12G = 12,  // +/- 12G range
    LIS3MDL_RANGE_16G = 16  // +/-16G range
} LIS3MDL_MeasRange_t;


// ------------------------------------
// LIS3MDL Magnetometer Sensor Object
// ------------------------------------
class LIS3MDL_Mag
{
public:
    LIS3MDL_Mag(TwoWire *userWire = &SENSOR_I2C);
    bool Initialize(LIS3MDL_MeasRange_t measRange = LIS3MDL_RANGE_4G);
    bool ReadSensor();
    float GetMx();
    float GetMy();
    float GetMz();
    float GetTemperature();
    uint32_t prevMeasMicros;  // Previous measurement micros()
protected:
private:
    float _mx;  // x-magnetometer reading [uT]
    float _my;  // y-magnetometer reading [uT]
    float _mz;  // z-magnetometer reading [uT]
    TwoWire *_SensorWire;
    LIS3MDL_MeasRange_t _range;  // Set meas. range
    void I2Cwrite8(uint8_t regOfInterest, uint8_t valToWrite);
    uint8_t I2Cread8(uint8_t regOfInterest);
};
