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
#include "constants.h"


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

    void UpdateGravity(float lat_rad = CONSTS_PIDIV4, float alt_msl = 200.0f);
    float GetGravity();


private:
    void _ComputeGravity(float lat, float alt);

    /* Private Variables */
    float _grav;  // Computed gravitational acceleration in [m/s/s]
};

// Only one instance of InertialNavSystem
extern GravityComputer &GravComputer;

