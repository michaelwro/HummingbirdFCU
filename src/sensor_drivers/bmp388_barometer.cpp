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


// ----------------------------------------------------------------------------
// BMP388Baro::BMP388Baro(TwoWire *wireInput)
// ----------------------------------------------------------------------------
/**
 * Create BMP388 pressure and temperature sensor object.
 * 
 * @param wireInput Wire/I2C interface that the sensor is connected to.
 */
BMP388Baro::BMP388Baro(TwoWire *wireInput)
{
    this->_SensorWire = wireInput;
    this->_p = 101325.0f;
    this->_t = 15.0f;
    this->prevMeasMicros = micros();
}


// ----------------------------------------------------------------------------
// BMP388Baro::Initialize(uint8_t presOS, uint8_t tempOS, 
//                            uint8_t iirCoef, uint8_t odr)
// ----------------------------------------------------------------------------
/**
 * Use Adafruit's BMP388 library to configure the BMP388. Take a few readings 
 * to 'flush out' bad data. The input config. param. definitions can be found 
 * in lib/Adafruit_BMP3XX/Adafruit_BMP3XX.h
 * 
 * @param presOS    Pressure oversampling factor.
 * @param tempOS    Temperature oversampling factor.
 * @param iirCoef   IIR filter coef. for smoothing out data.
 * @param odr       Output data rate.
 * @returns         True if good reading, false if error.
 */
bool BMP388Baro::Initialize(uint8_t presOS, uint8_t tempOS, 
                                uint8_t iirCoef, uint8_t odr)
{
    uint8_t i;
    uint8_t numFlushReadings = 5;


    // Connect to the sensor with Adafruit's code
    if (!this->begin(0x77, this->_SensorWire))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PRINTLN("BMP388BAROTEMP:BMP388BaroTemp ERROR: Cound not connect to BMP388. Check wiring and settings.");
        #endif
        this->connected = false;
    }

    // Set oversampling
    if (!this->setTemperatureOversampling(tempOS))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PRINTLN("BMP388BAROTEMP:Initialize ERROR: Could not set temp OS. Check settings.");
        #endif
        return false;
    }

    if (!this->setPressureOversampling(presOS))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PRINTLN("BMP388BAROTEMP:Initialize ERROR: Could not set pres OS. Check settings.");
        #endif
        return false;
    }

    // Set IIR coef.
    if (!this->setIIRFilterCoeff(iirCoef))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PRINTLN("BMP388BAROTEMP:Initialize ERROR: Could not set IIR filter coef. Check settings.");
        #endif
        return false;
    }

    // Set data rate
    if (!this->setOutputDataRate(odr))
    {
        #ifdef BMP388_DEBUG
        DEBUG_PRINTLN("BMP388BAROTEMP:Initialize ERROR: Could not set ODR. Check settings.");
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


// ----------------------------------------------------------------------------
// BMP388Baro::ReadSensor()
// ----------------------------------------------------------------------------
/**
 * Use Adafruit's BMP388 library to read temperature and pressure registers.
 * 
 * @returns True if good reading, false if error.
 */
bool BMP388Baro::ReadSensor()
{
    if (!this->performReading())
    {
        #ifdef BMP388_DEBUG
        DEBUG_PRINTLN("BMP388BAROTEMP:ReadSensor ERROR: Error reading sensor data.");
        #endif
        return false;
    }

    this->prevMeasMicros = micros();

    // Cvt. from double to float
    this->_p = (float)this->pressure;
    this->_t = (float)this->temperature;

    return true;
}


// ----------------------------------------------------------------------------
// BMP388Baro::GetPressure()
// ----------------------------------------------------------------------------
/**
 * Return atmospheric pressure in [Pa].
 * 
 * @returns Pressure in [Pa]
 */
float BMP388Baro::GetPressure()
{
    return this->_p;
}


// ----------------------------------------------------------------------------
// BMP388Baro::GetTemperature()
// ----------------------------------------------------------------------------
/**
 * Return atmospheric temperature in [C].
 * 
 * @returns Temperature in [C]
 */
float BMP388Baro::GetTemperature()
{
    return this->_t;
}
