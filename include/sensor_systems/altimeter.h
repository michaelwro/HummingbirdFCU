// ----------------------------------------------------------------------------
// BAROMETRIC ALTIMETER
// 
// Code by: Michael Wrona
// Created: 4 Sept 2021
// ----------------------------------------------------------------------------
// This is the class for creating a barometric altimeter. It uses the BMP388 
// driver code and some extra math to compute altitude changes.


#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "hummingbird_config.h"
#include "sensor_drivers/bmp388_barometer.h"
// #include "Adafruit_BMP3XX/Adafruit_BMP3XX.h"



// constexpr float ALTIMETER_SLP = 101325.0f;  ///< Sea-level pressure obtained from METAR report [Pa]



class BarometricAltimeter
{
public:
    BarometricAltimeter();
    ~BarometricAltimeter() {};

    bool Initialize();
    bool SetSLP(float pres);
    bool Update();

    float GetPressure();
    float GetAltitude();  // MSL
    float GetVerticalSpeed();
    float GetTemperature();

    float GetGroundPressure();
    float GetGroundTemperature();

    uint32_t prevUpdateMicros;  ///< Last update time micros() [us]
protected:
private:
};



