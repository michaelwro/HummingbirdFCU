// ----------------------------------------------------------------------------
// SENSOR CALIBRATION PARAMETERS
//
// Code By: Michael Wrona
// Created: 28 July 2020
// ----------------------------------------------------------------------------
/**
 * Header file to store sensor calibration coefficients.
 */

#pragma once


// ----------------------------------------------------------------------------
// Magnetometer calibration coefficients (data in [uT])
// | HX_CAL |   | S11 S12 S13 | | HX - BX|
// | HY_CAL | = | S12 S22 S23 | | HY - BY|
// | HZ_CAL |   | S13 S23 S33 | | HZ - BZ|
// ----------------------------------------------------------------------------
constexpr float SENSCALIB_MAG_S11   = 1.025278f;  // S11 magn. calib. coef.
constexpr float SENSCALIB_MAG_S12   = 0.047183f;  // S12 magn. calib. coef.
constexpr float SENSCALIB_MAG_S13   = -0.004394f;  // S13 magn. calib. coef.
constexpr float SENSCALIB_MAG_S22   = 0.991938f;  // S22 magn. calib. coef.
constexpr float SENSCALIB_MAG_S23   = 0.011496f;  // S23 magn. calib. coef.
constexpr float SENSCALIB_MAG_S33   = 1.106796f;  // S33 magn. calib. coef.
constexpr float SENSCALIB_MAG_BX    = 15.606863f;  // X magn. bias
constexpr float SENSCALIB_MAG_BY    = 38.011123f;  // Y magn. bias
constexpr float SENSCALIB_MAG_BZ    = 41.207505f;  // Z magn. bias


// ----------------------------------------------------------------------------
// Accelerometer calibration coefficients (data in [m/s/s])
// | AX_CAL |    | AX - BX |
// | AY_CAL | =  | AY - BY |
// | AZ_CAL |    | AZ - BZ |
// ----------------------------------------------------------------------------
constexpr float SENSCALIB_ACCEL_BX = 0.0f;
constexpr float SENSCALIB_ACCEL_BY = 0.0f;
constexpr float SENSCALIB_ACCEL_BZ = 0.0f;




// constexpr float SENSCALIB_MAG_SX = 0.973355689960329f;    // Soft iron X
// constexpr float SENSCALIB_MAG_SY = 1.005511516783f;   // Soft iron Y
// constexpr float SENSCALIB_MAG_SZ = 1.02174231248408f;    // Soft iron Z
// constexpr float SENSCALIB_MAG_BX = 29.1965507734923f;  // Hard iron offset X
// constexpr float SENSCALIB_MAG_BY = -40.9414327174498f;  // Hard iron offset Y
// constexpr float SENSCALIB_MAG_BZ = -28.0104222217251f;  // Hard iron offset Z
