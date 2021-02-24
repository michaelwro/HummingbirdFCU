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
 * - Initializing and configuring the sensor
 * - Reading pressure and temperature
 * - Filtering raw measurements
 * - Computing change in altitude from T/O location
 * - Computing vertical speed
 * 
 * The library applies a median filter to the data to smooth out measurements 
 * during 'hover' states, and based on thresholds, returns more responsive 
 * readings during altitude changes.
 * 
 * Computes vertical speed based on a backwards difference formula
 */


#ifndef BARO_ALTIMETER_H_
#define BARO_ALTIMETER_H_

#define BARO_ALTIMETER_DEBUG


// Includes
#include <Wire.h>
#include <math.h>
#include "hummingbird_config.h"
#include "Adafruit_BMP3XX.h"
#include "filters/median_filter.h"
#include "filters/low_pass_filter.h"


// Constants/config
constexpr float BARO_ALTIMETER_PRES_MAX = 108300.0f;  // [Pa] Max. allowable atmos. pressure (~32inHg)
constexpr float BARO_ALTIMETER_PRES_MIN = 94800.0f;  // [Pa] Min. allowable atmos. pressure (~28inHg)
constexpr float BARO_ALTIMETER_TEMP_MAX = 50.0f;  // [C] Max. allowable atmos. temperature (122F)
constexpr float BARO_ALTIMETER_TEMP_MIN = -23.0f;  // [C] Min. allowable atmos. temperature (-10F)


class BaroAltimeter:
public Adafruit_BMP3XX
{
    public:
        BaroAltimeter();
        bool ConnectToSensor(TwoWire *userWire = &Wire2);
        bool ConfigureSensorParams(
            uint8_t presOS = BMP3_NO_OVERSAMPLING, uint8_t tempOS = BMP3_NO_OVERSAMPLING,
            uint8_t iirCoef = BMP3_IIR_FILTER_COEFF_3, uint8_t sensODR = BMP3_ODR_50_HZ);
        bool Initialize();
        bool SetMSLPres(float presMSL_Pa = 101325.0f);
        // bool SetPresMedFiltWidth(uint8_t windowLen = 5);
        // bool SetTempLPFFactor(float alpha = 0.60f);
        // bool SetPresLPFFactor(float alpha = 0.985f);
        bool ReadSensor();
        float GetMSLPres();
        float GetAltitudeMSL();
        float GetAltitude();
        float GetLaunchAltMSL();
        float GetGroundPres();
        float GetGroundTemp();
        float GetPressure();
        float GetTemp();
        float GetVertSpeed();

        bool isMSLPSet;  // True if MSL pressure is set, false if not
        bool isConnected;  // True if sensor connection began, false if not
        bool isConfigured;  // True if sensor params are set, false if not
        bool isReady;  // True if connected with ground-level pres and temp are set, false if not
        // bool gotData;  // True if a measurement was taken, false if not
        
    private:

        bool _ReadGroundPresTemp(uint8_t n, unsigned long measDelay);
        bool _SetTakeoffAltitude();
        
        float _p;  // [Pa] Current pressure (filtered)
        float _pRaw;  // [Pa] Unfiltered pressure
        float _t;  // [C] Current temperature (filtered)
        float _tRaw;  // [C] Unfiltered temperature
        float _altMSL;  // [m] Altitude above MSL
        float _lastAltMSL;  // [m] Previous altitude above MSL. Used to calculate vertical speed.
        float _alt;  // [m] Altitude above takeoff location
        float _groundPres;  // [Pa] Ground-level/takeoff altitude pressure
        float _groundTemp;  // [C] Ground-level/takeoff altitude temperature
        float _groundAltMSL;  // [m] Ground-level/takeoff altitude above sea level
        float _mslPres;  // [Pa] Pressure at MSL, typically obtained from airport METAR reports
        float _vertSpeed;  // [m/s] Vertical speed
        unsigned long _lastMeasMillis;  // [ms] Last measurement time, used to compute dt
        unsigned long _currMeasMillis;  // [ms] Current measurement time, used to compute dt
        MedianFilter _PresFastFilter;
        LowPassFilter _PresSlowFilter;
        LowPassFilter _TempSlowFilter;

};



#endif  // BARO_ALTIMETER_H_
