// ----------------------------------------------------------------------------
// SENSOR CALIBRATION COEFFICIENTS
//
// Code By: Michael Wrona
// Created: 28 July 2020
// ----------------------------------------------------------------------------
/**
 * Header file to store sensor calibration coefficients.
 * 
 * Included:
 *     - Magnetometer mard and soft-iron corrections.
 *     - Initial gyroscope offsets
 */



#ifndef __SENSOR_CALIB_PARAMS_H__
#define __SENSOR_CALIB_PARAMS_H__


// ----------------------------------------------------------------------------
// Magnetometer calibration coefficients (data in [uT])
// | HX_CAL |   | S11 S12 S13 | | HX - BX|
// | HY_CAL | = | S12 S22 S23 | | HY - BY|
// | HZ_CAL |   | S13 S23 S33 | | HZ - BZ|
// ----------------------------------------------------------------------------
constexpr float SENSCALIB_MAG_S11   = 1.0f;  // S11 magn. calib. coef.
constexpr float SENSCALIB_MAG_S12   = 1.0f;  // S12 magn. calib. coef.
constexpr float SENSCALIB_MAG_S13   = 1.0f;  // S13 magn. calib. coef.
constexpr float SENSCALIB_MAG_S22   = 1.0f;  // S22 magn. calib. coef.
constexpr float SENSCALIB_MAG_S23   = 1.0f;  // S23 magn. calib. coef.
constexpr float SENSCALIB_MAG_S33   = 1.0f;  // S33 magn. calib. coef.
constexpr float SENSCALIB_MAG_BX    = 0.0f;  // X magn. bias
constexpr float SENSCALIB_MAG_BY    = 0.0f;  // Y magn. bias
constexpr float SENSCALIB_MAG_BZ    = 0.0f;  // Z magn. bias



// constexpr float SENSCALIB_MAG_SX = 0.973355689960329f;    // Soft iron X
// constexpr float SENSCALIB_MAG_SY = 1.005511516783f;   // Soft iron Y
// constexpr float SENSCALIB_MAG_SZ = 1.02174231248408f;    // Soft iron Z
// constexpr float SENSCALIB_MAG_BX = 29.1965507734923f;  // Hard iron offset X
// constexpr float SENSCALIB_MAG_BY = -40.9414327174498f;  // Hard iron offset Y
// constexpr float SENSCALIB_MAG_BZ = -28.0104222217251f;  // Hard iron offset Z


// ----------------------------------------------------------------------------
// Initial gyroscope biases
// Determined from experiments
// ----------------------------------------------------------------------------
// constexpr float SENSCALIB_GYRO_BX0 = 0.00385633f;  // Gyro x-bias initial guess [rad/s]
// constexpr float SENSCALIB_GYRO_BY0 = 0.002484f;    // Gyro y-bias initial guess [rad/s]
// constexpr float SENSCALIB_GYRO_BZ0 = 0.00212967f;  // Gyro z-bias initial guess [rad/s]




#endif  // __SENSOR_CALIB_PARAMS_H__