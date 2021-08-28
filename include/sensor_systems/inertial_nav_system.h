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
#include "debugging.h"
#include "maths/vectors.h"
#include "gravity_computer.h"
#include "hummingbird_config.h"
#include "sensor_drivers/fxas21002_gyro.h"
#include "sensor_drivers/fxos8700_accelmag.h"
#include "sensor_drivers/sensor_calib_params.h"
#include "filters/low_pass_filter.h"


#ifdef DEBUG
    #define INS_DEBUG  // Print INS debug messages to the debug port.
#endif

/* Filters */
constexpr float INS_ACCEL_LPF_SF = 0.95f;  // Smoothing factor (alpha) of accelerometer low-pass filter, [0, 1]
// constexpr float INS_GYRO_LPF_SF = 0.98f;  // Gyro low pass filter smoothing factor [0, 1]

/* Turn-on biases */
constexpr uint32_t INS_BIAS_INIT_TIME = 1000;  // [millisec] Amount of time taken to determine accel. and gyro turn-on bias

/* Measurement ranges */
constexpr GyroRanges_t INS_GYRO_RANGE = GYRO_RNG_1000DPS;  // Gyro measurement range
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
    Vectorf GyroRaw;     // [deg/s], [gx, gy, gz] Raw gyro measurements
    Vectorf GyroTOBias;  // [rad/s], [bgx, bgy, bgz] Measured gyro turn-on biases
    Vectorf Accel;       // [m/s/s], [ax, ay, az] Accelerometer measurements (filtered)
    Vectorf AccelRaw;    // [g's], [ax, ay, az] Raw accelerometer measurements
    Vectorf AccelTOBias; // [m/s/s], [bax, bay, baz] Measured accelerometer turn-on biases
    uint32_t prevUpdateMicros;  // [us] Previous INS update micros()
protected:
private:
    void UpdateAccelAngles();
    bool MeasureInitGyroBiases(uint32_t samplePeriod);
    bool MeasureInitAccelBiases(uint32_t samplePeriod);

    float roll;     // [rad] Accelerometer roll angle (NED)
    float pitch;    // [rad] Accelerometer pitch angle (NED)
    
    FXOS8700AccelMag AccelMagSensor;  // Accelerometer/magnetometer sensor class
    FXAS21002Gyro GyroSensor;  // Gyroscope sensor class
    LowPassFilter AxLPF;  // Ax data filter
    LowPassFilter AyLPF;  // Ay data filter
    LowPassFilter AzLPF;  // Az data filter
};



// Only one instance of InertialNavSystem
extern InertialNavSystem &INS;
