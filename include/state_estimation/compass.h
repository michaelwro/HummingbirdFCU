// ----------------------------------------------------------------------------
// MAGNETOMETER/COMAPSS OBJECT
// 
// Code By: Michael Wrona
// Created : 4 Mar 2021
// ----------------------------------------------------------------------------
/**
 * The compass class is in charge of taking magnetometer measurements and 
 * computing tilt-compensated (magnetic) heading 
 */

#pragma once


#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "hummingbird_config.h"
#include "sensors/lis3mdl_magnetometer.h"
#include "maths/vectors.h"
#include "constants.h"
#include "maths/math_functs.h"
#include "sensors/sensor_calib_params.h"


#if defined(DEBUG) && defined(DEBUG_PORT)
    /* Print compass debug messages to the debug port. Both DEBUG and DEBUG_PORT in hummingbird_config.h must be defined. */
    #define MAGCOMPASS_DEBUG
#endif


/* CONFIGURATION PARAMETERS */
constexpr LIS3MDL_MeasRange_t MAGCOMPASS_RANGE  = LIS3MDL_RANGE_4G;  // Magnetometer measurement range

/**
 * Decide whether or not to rotate magnetometer measurements.
 * The Magnetometer in the GPS is upside down and rotated. By default, apply 
 * rotations to align measurements with the body frame.
 */
// #define MAGCOMPASS_DO_NOT_ROTATE


class MagCompass
{
public:
    MagCompass(TwoWire *userWire = &SENSOR_I2C);
    ~MagCompass();
    
    // Do not allow copies
    static MagCompass &GetInstance();
    MagCompass(const MagCompass &) = delete;
    MagCompass &operator=(const MagCompass &) = delete;

    bool Initialize();
    bool Update();
    float GetHeading(Vectorf AccelMeas);

    uint32_t prevUpdateMicros;  // [us] Previous update micros()
    Vectorf Mag;     // [mx, my, mz], [uT] Magnetometer readings (calibrated)
    Vectorf MagRaw;  // [mx, my, mz], [uT] Raw, uncalibrated readings

protected:
private:
    float heading;  // [rad], [0, 2pi) Tilt-compensated magnetic heading
    LIS3MDL_Mag MagSensor;
    LIS3MDL_MeasRange_t magMeasRange;
    TwoWire *SensorWire;
};

// Only one instance of MagCompass
extern MagCompass &Compass;