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
// GravityComputer(float latRecomputeThres, 
//                float altRecomputeThres)
// --------------------------------------------------------------
/**
 * Compute gravitational acceleration as a function of latitude 
 * and altitude above mean sea level. Incorporates thresholding to 
 * save on computations. Gravity is recomputed only if a change 
 * in latitude OR altitude exceeds a threshold set by the user. 
 * Down is positive!
 * 
 * @param latRecomputeThres [deg] Latitude change threshold. 
 *                          Default is 0.0005 deg. (~50m).
 * @param altRecomputeThres [meters] Altitude change threshold. 
 *                          Default is 5 meters.
 */
GravityComputer::GravityComputer(float latRecomputeThres, float altRecomputeThres)
{
    // Check that inputs are positive
    if (latRecomputeThres <= 0.0f)
        latRecomputeThres *= -1.0f;
    
    if (altRecomputeThres <= 0.0f)
        altRecomputeThres *= -1.0f;
    
    // Convert units and assign values
    this->_latThres = latRecomputeThres * DEG2RAD;  // degrees to radians
    this->_altThres = altRecomputeThres;

    this->_grav = CONSTS_GRAV;
}


// --------------------------------------------------------------
// UpdateGravity(float lat_rad, float alt_msl)
// --------------------------------------------------------------
/**
 * Compute gravitational acceleration at a given latitude in 
 * [rad] and altitude above mean sea level in [m]. Down is 
 * positive.
 * 
 * @param lat_rad   Latitude in radians. Default 45deg = PI/4
 * @param alt_msl   Altitude above MSL in meters. Default 200m.
 */
void GravityComputer::UpdateGravity(float lat_rad, float alt_msl)
{
    bool recompute = false;
    
    if (fabsf(lat_rad - this->_prevLat) >= this->_latThres ||   // If latitude change is greater than the threshold _latThres
        fabsf(alt_msl - this->_prevAlt) >= this->_altThres      // If altitude change is greater than the threshold _altThres
        ) recompute = true;
    
    // Update gravity, if required
    if (recompute == true)
    {
        this->_ComputeGravity(lat_rad, alt_msl);
        this->_prevLat = lat_rad;  // Replace old with new
        this->_prevAlt = alt_msl;
    }
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
    float term1, term2, g0, denom;
    float sinLat = sinf(lat);
    float sin2Lat = sinf(2.0f * lat);
    
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
