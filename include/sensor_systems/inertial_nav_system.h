// ----------------------------------------------------------------------------
// ACCELEROMETER, GYRO, AND MAGNETOMETER INERTIAL SENSOR SYSTEM
// 
// Code By: Michael Wrona
// Created: 27 Feb 2021
// ----------------------------------------------------------------------------
/**
 * The inertial sensor software system brings together the accelerometer  
 * and gyro, sensors to form the Inertial Navigation System. This system 
 * provides an interface to manage sensor readings, filtering raw measurements 
 * for noise, and applying calibration parameters.
 */

#pragma once


#include <Wire.h>
#include <Arduino.h>
#include <math.h>
#include "maths/vectors.h"
#include "gravity_computer.h"
#include "hummingbird_config.h"
#include "sensor_drivers/fxas21002_gyro.h"
#include "sensor_drivers/fxos8700_accelmag.h"
#include "sensor_drivers/sensor_calib_params.h"
#include "filters/median_filter.h"
#include "filters/low_pass_filter.h"


#if defined(DEBUG) && defined(DEBUG_PORT)
    /* Print INS debug messages to the debug port. Both DEBUG and DEBUG_PORT in hummingbird_config.h must be defined. */
    #define INS_DEBUG
#endif


/* CONFIGURATION PARAMETERS */
constexpr size_t INS_ACCEL_MFILT_LEN    = 3;  // Size/length of accelerometer median filter
constexpr float INS_GYRO_LPF_SF         = 0.99f;  // Gyro low pass filter smoothing factor [0, 1]

/* [sec] Amount of time taken to determine accel. and gyro turn-on bias */
constexpr float INS_BIAS_INIT_TIME = 1.0f;

/* Set gyro and accelerometer measurement ranges */
constexpr GyroRanges_t INS_GYRO_RANGE   = GYRO_RNG_1000DPS;  // Gyro measurement range
constexpr AccelRanges_t INS_ACCEL_RANGE = ACCEL_RNG_4G;  // Accelerometer measurement range


// ----------------------------------------------------------------------------
// InertialNavSystem()
// ----------------------------------------------------------------------------
/**
 * Constructor for the InertialNavSystem class. This system provides an 
 * interface to manage sensor readings, filtering raw measurements for noise, 
 * and applying calibration parameters.
 */
class InertialNavSystem
{
public:
    InertialNavSystem();
    ~InertialNavSystem();

    // Do not allow copies (singleton)
    static InertialNavSystem &GetInstance();  // Accessor
    InertialNavSystem(const InertialNavSystem &) = delete;
    InertialNavSystem &operator=(const InertialNavSystem &) = delete;

    void SetSampleRate(float fs_hz);
    bool Initialize();
    bool Update();
    float GetAccelPitch();
    float GetAccelRoll();
    
    Vectorf Gyro;        // [rad/s], [gx, gy, gz] Gyro measurements (filtered) 
    Vectorf GyroRaw;     // [rad/s], [gx, gy, gz] Raw gyro measurements
    Vectorf GyroTOBias;  // [rad/s], [bgx, bgy, bgz] Measured gyro turn-on biases
    Vectorf Accel;       // [m/s/s], [ax, ay, az] Accelerometer measurements (filtered)
    Vectorf AccelRaw;    // [m/s/s], [ax, ay, az] Raw accelerometer measurements
    Vectorf AccelTOBias; // [m/s/s], [bax, bay, baz] Measured accelerometer turn-on biases
    uint32_t prevUpdateMicros;  // [us] Previous INS update micros()
protected:
private:
    void UpdateAccelAngles();
    bool ComputeInitGyroBiases(size_t n);
    bool ComputeInitAccelBiases(size_t n);

    uint32_t tsMillis;          // [ms] INS sample period
    float roll;     // [rad] Accelerometer roll angle (NED)
    float pitch;    // [rad] Accelerometer pitch angle (NED)
    float fs;       // [Hz] INS sample rate
    float ts;       // [sec] INS sample period
    
    FXOS8700AccelMag AccelMagSensor;  // Accelerometer/magnetometer sensor class
    FXAS21002Gyro GyroSensor;  // Gyroscope sensor class
    MedianFilter AxMFilt;  // Ax data filter
    MedianFilter AyMFilt;  // Ay data filter
    MedianFilter AzMFilt;  // Az data filter
};



// Only one instance of InertialNavSystem
extern InertialNavSystem &INS;
