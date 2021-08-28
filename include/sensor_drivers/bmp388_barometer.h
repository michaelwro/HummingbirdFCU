// ----------------------------------------------------------------------------
// BMP388 BAROMETRIC ALTIMETER AND TEMPERATURE SENSOR
// 
// Code By: Michael Wrona
// Created: 25 Feb 2021
// ----------------------------------------------------------------------------
/**
 * This is driver code for the BMP388 temp/pres sensor. It also incorporates 
 * Adafruit's BMP388 driver code.
 */

#pragma once


#include <Arduino.h>
#include <Wire.h>
#include "hummingbird_config.h"
#include "debugging.h"
// #include "constants.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP3XX.h"

#if defined(DEBUG)
    #define BMP388_DEBUG  // Enable/disable debug print messages
#endif


class BMP388Baro:
public Adafruit_BMP3XX
{
public:
    BMP388Baro(TwoWire *wireInput = &SENSOR_I2C);
    bool Initialize(uint8_t presOS=BMP3_NO_OVERSAMPLING, uint8_t tempOS=BMP3_NO_OVERSAMPLING, 
                    uint8_t iirCoef=BMP3_IIR_FILTER_COEFF_3, uint8_t odr=BMP3_ODR_50_HZ);
    bool ReadSensor();
    float GetPressure();
    float GetTemperature();
    uint32_t prevMeasMicros;  // [us] Previous measurement micros()
protected:
private:
    bool connected;  // Whether or not a good connection was made
    float _t;  // [C] Raw temperature
    float _p;  // [Pa] Raw pressure
    TwoWire *_SensorWire;  // I2C interface that the sensor is connected to.
};
