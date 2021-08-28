// ----------------------------------------------------------------------------
// GRAVITATIONAL ACCELERATION COMPUTER
// 
// Code By: Michael Wrona
// Created: 20 Feb 2021
// ----------------------------------------------------------------------------
/**
 * Compute the gravitational acceleration while in flight. Computes gravity as 
 * a function of latitude, and altitude above mean sea level. Uses 
 * a simple gravity model found in:
 * Titterton, D. H. Weston, J. L. "Strapdown Inertial Navigation Technology," 
 * 2nd ed. The Institution of Electrical Engineers, 2004. (pg. 57)
 */

#pragma once


#include <math.h>
#include "hummingbird_config.h"
#include "conversions.h"
#include "debugging.h"
#include "constants.h"


// Use the WGS84 gravity formula (https://en.wikipedia.org/wiki/Gravity_of_Earth#:~:text=various%20cities%20show-,Mathematical%20models,-%5Bedit%5D)
// #define GRAV_COMPUTER_WGS84_MODEL

// Use the full Free-Air Correction model. Uses the linearized one by default.
// #define GRAV_COMPUTER_NONLINEAR_FAC


// --------------------------------------------------------------
// GravityComputer()
// --------------------------------------------------------------
/**
 * Compute gravitational acceleration as a function of latitude 
 * and altitude above mean sea level. Down is positive!
 */
class GravityComputer
{
public:
    GravityComputer();
    ~GravityComputer();
    // Do not allow copies (singleton)
    static GravityComputer &GetInstance();  // Accessor
    GravityComputer(const GravityComputer &) = delete;
    GravityComputer &operator=(const GravityComputer &) = delete;

    bool Update(float lat_rad = CONSTS_PIDIV4, float lon_rad = 0.0f, float alt_msl = 280.0f);
    float GetGravity();
    uint32_t errCount;  // Number of errors or invalid inputs to the code.

private:
    bool _ComputeGravity(float lat, float lon, float alt);
    
    float _grav;  // Computed gravitational acceleration in [m/s/s]
};

// Only one instance of InertialNavSystem
extern GravityComputer &GravComputer;
