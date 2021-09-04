// ----------------------------------------------------------------------------
// BAROMETRIC ALTIMETER CLASS
// 
// Code By: Michael Wrona
// Created: 5 Feb 2021
// ----------------------------------------------------------------------------
/**
 * This class encompasses all functions to create a barometric altimeter for 
 * the drone, based on Adafruit's BMP388 sensor library. This class is 
 * responsible for:
 * - Initializing the sensor
 * - Reading pressure and temperature
 * - Filtering raw measurements
 * - Computing change in altitude from T/O location
 * - Computing vertical speed
 */


#include <Arduino.h>
#include "baro_altimeter.h"





BaroAltimeter::BaroAltimeter()
: _PresFastFilter(7, 101325.0f)
{
    // Set default values for variables or zero them
    this->isConnected = false;
    this->isReady = false;
    this->isConfigured = false;
    this->isMSLPSet = false;
    // this->gotData = false;
    this->_p = 101325.0f;
    this->_t = 15.0f;
    this->_pRaw = 101325.0f;
    this->_tRaw = 15.0f;
    this->_groundPres = 101325.0f;
    this->_groundTemp = 15.0f;
    this->_groundAltMSL = 280.0f;
    this->_mslPres = 101325.0f;
    this->_vertSpeed = 0.0f;
    this->_lastMeasMillis = 0;
    this->_currMeasMillis = 0;

    // this->_PresFastFilter.SetWindowWidth(7);
    this->_TempSlowFilter.SetSmoothingFactor(0.1f);
    this->_PresSlowFilter.SetSmoothingFactor(.015f);
}


// ----------------------------------------------------------------------------
// ConnectToSensor(TwoWire *userWire)
// ----------------------------------------------------------------------------
/**
 * Attempt to connect to the BMP388 pressure/temperature sensor by running 
 * Adafruit's BMP_3XX.begin() method. Returns false if it could't connect to 
 * the sensor.
 *
 * @param userWire  I2C bus that the sensor is connected to. Default Wire2.
 * @return          True if success, false if not.
 */
bool BaroAltimeter::ConnectToSensor(TwoWire *userWire)
{
    if (!this->begin((uint8_t)0x77, userWire))
    {
        // Sensor could not be initialized correctly
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Cound not connect to BMP388. Check wiring and settings.");
        #endif
        return false;
    }

    this->isConnected = true;
    return true;
}


// ----------------------------------------------------------------------------
// ConfigureSensorParams(
//     uint8_t presOS, uint8_t tempOS,
//     uint8_t iirCoef, uint8_t sensODR)
// ----------------------------------------------------------------------------
/**
 * Configure BMP388 oversampling, IIR filtering, and ODR. Refer to 
 * Adafruit_BMP_3XX.h for config. options
 * 
 * @param presOS    Pressure oversampling
 * @param tempSO    Temperature oversampling
 * @param iirCoef   Infinite impulse response (IIR) filter coef.
 * @param sensODR   Output data rate [Hz]
 * @return          True if successfully configured, false if not.
 */
bool BaroAltimeter::ConfigureSensorParams(
    uint8_t presOS, uint8_t tempOS,
    uint8_t iirCoef, uint8_t sensODR)
{
    // Check if we are connected to the sensor. Otherwise, connect to it
    if (!this->isConnected)
        this->ConnectToSensor(&Wire2);

    if (!this->setTemperatureOversampling(tempOS))
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Could not set temp OS. Check settings.");
        #endif
        return false;
    }

    if (!this->setPressureOversampling(presOS))
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Could not set pres OS. Check settings.");
        #endif
        return false;
    }

    if (!this->setIIRFilterCoeff(iirCoef))
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Could not set IIR filter coef. Check settings.");
        #endif
        return false;
    }

    if (!this->setOutputDataRate(sensODR))
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Could not set ODR. Check settings.");
        #endif
        return false;
    }

    this->isConfigured = true;
    return true;
}



bool BaroAltimeter::SetMSLPres(float presMSL_Pa)
{
    if (presMSL_Pa >= BARO_ALTIMETER_PRES_MAX || presMSL_Pa <= BARO_ALTIMETER_PRES_MIN)
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Specified MSL pressure out of bounds. Check value.");
        #endif
        return false;
    }
    
    this->_mslPres = RangeConstrain(presMSL_Pa, BARO_ALTIMETER_PRES_MIN, BARO_ALTIMETER_PRES_MAX);
    this->isMSLPSet = true;
    return true;
}




// ----------------------------------------------------------------------------
// Initialize()
// ----------------------------------------------------------------------------
/** Initialize the altimeter class. Measures ground temperature and pressure 
 * and sets takeoff altitude parameters.
 * 
 * @return      True if successful, flase if not.
 */
bool BaroAltimeter::Initialize()
{
    // Check if connected
    if (this->isConnected == false)
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Call ConnectToSensor() method before intializing. Using default Wire2...");
        #endif
        if (!this->ConnectToSensor())  // Use defaults in baro_altimeter.h
            return false;
    }

    // Check if config'd
    if (this->isConfigured == false)
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Call ConfigureSensorParams() method before intializing. Using default params...");
        #endif
        if (!this->ConfigureSensorParams())  // Use defaults in baro_altimeter.h
            return false;
    }

    // Check if MSL pressure is set
    if (this->isMSLPSet == false)
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Call SetMSLPres() method before initializing. Using default value...");
        #endif
        if (!this->SetMSLPres())  // Use defaults in baro_altimeter.h
            return false;
    }

    // Read ground-level pressure and temperature
    if (!this->_ReadGroundPresTemp(50, 50))  // 50 readings * 50ms delay = 1.5 seconds
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Error reading ground pres and temp.");
        #endif
        return false;
    }

    // Set T/O Altitude MSL
    this->_SetTakeoffAltitude();

    #ifdef DEBUG
        DEBUG_PORT.println("BARO_ALTIMETER INITIALIZED:");
        DEBUG_PORT.print("    "); DEBUG_PORT.print(this->_groundPres, 2); DEBUG_PORT.println(" Pa  ");
        DEBUG_PORT.print("    "); DEBUG_PORT.print(this->_groundTemp, 2); DEBUG_PORT.println(" C");
        DEBUG_PORT.print("    "); DEBUG_PORT.print(this->_groundAltMSL, 2); DEBUG_PORT.println(" m");
    #endif

    this->isReady = true;
    return true;
}


// ----------------------------------------------------------------------------
// ReadSensor()
// ----------------------------------------------------------------------------
/**
 * Read raw pressure and temperature data from BMP388.
 * 
 * @return  True if a valid reading was done, false if not.
 */
bool BaroAltimeter::ReadSensor()
{
    float pfast;  // [Pa] "fast reacting" pressure reading from median filter
    float pdiff;  // [Pa] Pressure difference
    float presRatio;  // [Pa] ratio between current and MSL pressure. Used to compute altitude


    if (!this->performReading())
    {
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Error reading sensor data.");
        #endif
        return false;
    }

    // Cvt. rom double to float
    this->_pRaw = (float)this->pressure;
    this->_tRaw = (float)this->temperature;

    // Range checks on variables
    if (this->_pRaw >= BARO_ALTIMETER_PRES_MAX || this->_pRaw <= BARO_ALTIMETER_PRES_MIN)
    {
        this->_pRaw = 101325.0f;
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Pressure reading out of allowable bounds.");
        #endif
    }

    if (this->_tRaw >= BARO_ALTIMETER_TEMP_MAX || this->_tRaw <= BARO_ALTIMETER_TEMP_MIN)
    {
        this->_tRaw = 15.0f;
        #ifdef DEBUG
            DEBUG_PORT.println("BARO_ALTIMETER ERROR: Temperature reading out of allowable bounds.");
        #endif
    }

    /* UPDATE PRESSURE AND TEMPERATURE */
    this->_currMeasMillis = millis();

    // Filter pressure and temperature measurements
    // pfast = this->_PresFastFilter.Update(this->_pRaw);
    this->_t = this->_TempSlowFilter.Filter(this->_tRaw);
    this->_p = this->_PresSlowFilter.Filter(this->_pRaw);; //->_PresSlowFilter.FilterWithSF(pfast);

    // Depending on pressure change, select the "slow reacting" pressure for when the drone is "stationary"
    // or the "fast reacting" pressure when the drone changes altitude
    // Method can be seen at: https://youtu.be/2BLb6qUKikI?t=667

    /* UPDATE ALTITUDE */
    // TODO: Check if altitude is negative?
    presRatio = this->_p / this->_mslPres;
    // this->_altMSL = 153.8462f * (this->_groundTemp + 273.15f) * (1.0f - expf(0.190259f * logf(presRatio)));
    this->_altMSL = 44300.0f * (1.0f - powf(presRatio, 0.19f));
    this->_alt = this->_altMSL - this->_groundAltMSL;  // Above ground

    /* UPDATE VERTICAL SPEED */
    // Backwards difference formula, for now
    // TODO: Check for NaN or div. by zero error!
    this->_vertSpeed = ((this->_altMSL - this->_lastAltMSL) * 1000.0f) / (this->_currMeasMillis - this->_lastMeasMillis);
    
    /* UPDATE "CHANGE" VARIABLES */
    this->_lastAltMSL = this->_altMSL;
    this->_lastMeasMillis = this->_currMeasMillis;


    #ifdef BARO_ALTIMETER_DEBUG
        DEBUG_PORT.print("P: "); DEBUG_PORT.print(this->_p, 2);
        DEBUG_PORT.print("  T: "); DEBUG_PORT.print(this->_t, 2);
        DEBUG_PORT.print("  ALT: "); DEBUG_PORT.print(this->_alt, 2);
        DEBUG_PORT.print("  VS: "); DEBUG_PORT.println(this->_vertSpeed * 100.0f, 2);
    #endif

    return true;
}



/* Return "smoothed" atmospheric pressure in [Pa] */
float BaroAltimeter::GetPressure()
{
    return this->_p;
}


/* Return "smoothed" temperature in [C] */
float BaroAltimeter::GetTemp()
{
    return this->_t;
}


/* Return vertical speed in [m/s]. Up is positive */
float BaroAltimeter::GetVertSpeed()
{
    return this->_vertSpeed;
}


/* Return ground/takeoff pressure in [Pa] */
float BaroAltimeter::GetGroundPres()
{
    return this->_groundPres;
}


/* Return ground/takeoff temperature in [C] */
float BaroAltimeter::GetGroundTemp()
{
    return this->_groundTemp;
}


/* Return ground/takeoff pressure altitude in [m] */
float BaroAltimeter::GetLaunchAltMSL()
{
    return this->_groundAltMSL;
}


/* Return altitude above the ground/takeoff in [m] */
float BaroAltimeter::GetAltitude()
{
    return this->_alt;
}


/* Return pressure at mean sea level (set by user) in [Pa] */
float BaroAltimeter::GetMSLPres()
{
    return this->_mslPres;
}


/* Return altitude above mean sea level from altimeter in [m] */
float BaroAltimeter::GetAltitudeMSL()
{
    return this->_altMSL;
}



// ------------------------------------
// PRIVATE METHODS
// ------------------------------------



// ----------------------------------------------------------------------------
// _SetTakeoffAltitude()
// ----------------------------------------------------------------------------
/**
 * Compute the altitude above MSL at the T/O location. Uses equation from 
 * Ardupilot's source code.
 */
bool BaroAltimeter::_SetTakeoffAltitude()
{
    float presRatio = this->_groundPres / this->_mslPres;
    // this->_groundAltMSL = 153.8462f * (this->_groundTemp + 273.15f) * (1.0f - expf(0.190259f * logf(presRatio)));
    this->_groundAltMSL = 44300.0f * (1.0f - powf(presRatio, 0.19f));
    return true;
}


// ----------------------------------------------------------------------------
// _ReadGroundPresTemp(uint8_t n, unsigned long measDelay)
// ----------------------------------------------------------------------------
/**
 * Measure the ground-level pressure and temperature by taking the average of 
 * a few readings.
 * 
 * @param n     Number of measurements to take
 * @param measDelay Arduino 'delay()' between measurements in milliseconds
 * @return      True if success, false if not.
 */
bool BaroAltimeter::_ReadGroundPresTemp(uint8_t n, unsigned long measDelay)
{
    uint8_t i;
    this->_groundPres = 0.0f;
    this->_groundTemp = 0.0f;

    // Take a few readings to "flush out bad data"
    for (i = 0; i < 5; i++)
    {
        if (!this->performReading())
            return false;
        delay(measDelay);
    }
    // Take 'n' measurements and compute the average
    for (i = 0; i < n; i++)
    {
        if (!this->performReading())
            return false;

        this->_groundPres += (float)this->pressure;  // Adafruit returns double, cvt. to float
        this->_groundTemp += (float)this->temperature;
        delay(measDelay);
    }

    // TODO: Check average results?
    this->_groundPres /= (float)n;
    this->_groundTemp /= (float)n;

    return true;
}
