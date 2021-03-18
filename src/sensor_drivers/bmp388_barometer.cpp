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

#include "sensor_drivers/bmp388_barometer.h"



BMP388BaroTemp::BMP388BaroTemp(TwoWire *wireInput)
{
    this->connected = true;
    // Connect to the sensor with Adafruit's code
    if (!this->begin((uint8_t)0x77, wireInput))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PORT.println("BMP388BAROTEMP:BMP388BaroTemp ERROR: Cound not connect to BMP388. Check wiring and settings.");
        #endif
        this->connected = false;
    }

    this->_p = 101325.0f;
    this->_t = 15.0f;
    this->prevMeasMicros = micros();
}


bool BMP388BaroTemp::Initialize(uint8_t presOS, uint8_t tempOS, 
                                uint8_t iirCoef, uint8_t sensODR)
{
    uint8_t i;
    uint8_t numFlushReadings = 5;


    // If NOT connected, return false
    if (!this->connected)
    {
        #ifdef BMP388_DEBUG
        DEBUG_PORT.println("BMP388BAROTEMP:Initialize ERROR: Cound not connect to BMP388. Check wiring and settings.");
        #endif
        return false;
    }

    // Set oversampling
    if (!this->setTemperatureOversampling(tempOS))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PORT.println("BMP388BAROTEMP:Initialize ERROR: Could not set temp OS. Check settings.");
        #endif
        return false;
    }

    if (!this->setPressureOversampling(presOS))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PORT.println("BMP388BAROTEMP:Initialize ERROR: Could not set pres OS. Check settings.");
        #endif
        return false;
    }

    // Set IIR coef.
    if (!this->setIIRFilterCoeff(iirCoef))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PORT.println("BMP388BAROTEMP:Initialize ERROR: Could not set IIR filter coef. Check settings.");
        #endif
        return false;
    }

    // Set data rate
    if (!this->setOutputDataRate(sensODR))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PORT.println("BMP388BAROTEMP:Initialize ERROR: Could not set ODR. Check settings.");
        #endif
        return false;
    }


    // Take a few readings to flush out bad data
    for (i = 0; i < numFlushReadings; i++)
    {
        this->ReadSensor();
        delay(50);
    }

    return true;
}



bool BMP388BaroTemp::ReadSensor()
{
    if (!this->performReading())
    {
        #ifdef BMP388_DEBUG
        DEBUG_PORT.println("BMP388BAROTEMP:ReadSensor ERROR: Error reading sensor data.");
        #endif
        return false;
    }

    this->prevMeasMicros = micros();

    // Cvt. from double to float
    this->_p = (float)this->pressure;
    this->_t = (float)this->temperature;

    return true;
}


float BMP388BaroTemp::GetPressure()
{
    return this->_p;
}


float BMP388BaroTemp::GetTemperature()
{
    return this->_t;
}