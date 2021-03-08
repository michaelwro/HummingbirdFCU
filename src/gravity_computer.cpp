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

#include "gravity_computer.h"


// --------------------------------------------------------------
// GravityComputer()
// --------------------------------------------------------------
/**
 * Compute gravitational acceleration as a function of latitude 
 * and altitude above mean sea level. Down is positive!
 */
GravityComputer::GravityComputer()
{
    this->_grav = CONSTS_GRAV;
}


// --------------------------------------------------------------
// UpdatePosition(float lat_rad, float alt_msl)
// --------------------------------------------------------------
/**
 * Recompute gravity with updated latitude and altitude.
 * 
 * @param lat_rad   [rad] Latitude, default 45deg = PI/4
 * @param alt_msl   [m] Altitude above MSL, default 200m.
 */
void GravityComputer::UpdatePosition(float lat_rad, float alt_msl)
{
    this->_ComputeGravity(lat_rad, alt_msl);
}


// --------------------------------------------------------------
// GetGravity()
// --------------------------------------------------------------
/**
 * Returns gravitational acceleration in [m/s/s]. Down is 
 * positive. Be sure to periodically call UpdateGravity().
 * 
 * @return      Gravity in [m/s/s]
 */
float GravityComputer::GetGravity()
{
    return this->_grav;
}


/* Compute gravitational acceleration in [m/s/s] */
void GravityComputer::_ComputeGravity(float lat, float alt)
{
    float term1;
    float term2;
    float g0;
    float denom;
    float sinLat;
    float sin2Lat;

    sinLat = sinf(lat);
    sin2Lat = sinf(2.0f * lat);
    
    // Compute sea-level gravity
    term1 = 0.0053024f * sinLat * sinLat;
    term2 = 0.0000059f * sin2Lat * sin2Lat;
    g0 = 9.780318f * (1.0f + term1 - term2);  // Get grav. at sea level

    // Account for altitude
    denom = (1.0f + (alt / CONSTS_WGS84_A));
    this->_grav = g0 / (denom * denom);
}


/* Deconstructor */
GravityComputer::~GravityComputer() {}

/* GetInstance() function for singleton. Don't use as a user. */
GravityComputer &GravityComputer::GetInstance()
{
    static GravityComputer instance;
    return instance;
}

GravityComputer &GravComputer = GravComputer.GetInstance();
