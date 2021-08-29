// ----------------------------------------------------------------------------
// GRAVITATIONAL ACCELERATION COMPUTER
// 
// Code By: Michael Wrona
// Created: 20 Feb 2021
// ----------------------------------------------------------------------------
// Compute the gravitational acceleration while in flight. Computes gravity as 
// a function of latitude, longitude, and altitude above mean sea level.


#pragma once

#include <math.h>
#include "hummingbird_config.h"
#include "conversions.h"
#include "debugging.h"
#include "constants.h"
#include "filters/median_filter.h"


// #define GRAV_COMPUTER_WGS84_MODEL ///< Use the WGS84 gravity formula (https://en.wikipedia.org/wiki/Gravity_of_Earth#:~:text=various%20cities%20show-,Mathematical%20models,-%5Bedit%5D)
// #define GRAV_COMPUTER_NONLINEAR_FAC  //< Use the full Free-Air Correction model. Uses the linearized one by default.


/**
 * Gravity computer class
 */
class GravityComputer
{
public:
    GravityComputer();
    ~GravityComputer() {};
    // Do not allow copies (singleton)
    static GravityComputer &GetInstance();  // Accessor
    GravityComputer(const GravityComputer &) = delete;
    GravityComputer &operator=(const GravityComputer &) = delete;

    bool Update(float lat_rad = CONSTS_PIDIV4, float lon_rad = 0.0f, float alt_msl = 280.0f);
    float GetGravity();
    uint32_t errCount;  ///< Number of errors or invalid inputs to the code.

private:
    bool _ComputeGravity(float lat, float lon, float alt);
    float _grav;  ///< Computed gravitational acceleration in [m/s/s]
    MedianFilter GravSmoother;  ///< Median filter to smooth out gravity
};

// Only one instance of InertialNavSystem
extern GravityComputer &GravComputer;
