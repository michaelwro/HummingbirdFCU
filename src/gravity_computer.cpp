// ----------------------------------------------------------------------------
// GRAVITATIONAL ACCELERATION COMPUTER
// 
// Code By: Michael Wrona
// Created: 20 Feb 2021
// ----------------------------------------------------------------------------
// Compute the gravitational acceleration while in flight. Computes gravity as 
// a function of latitude, longitude, and altitude above mean sea level.

#include "gravity_computer.h"


/**
 * Compute gravitational acceleration as a function of latitude 
 * and altitude above mean sea level. Down is positive!
 */
GravityComputer::GravityComputer()
: GravSmoother(5, CONSTS_GRAV)
{
    this->_grav = CONSTS_GRAV;
    this->errCount = 0;
}


/**
 * Use a gravitymodel to compute gravity at a given latitude, longitude, and altitude.
 * 
 * @param lat_rad [rad] Geodetic latitude.
 * @param lon_rad [rad] Geodetic longitude.
 * @param alt_msl [m] Altitude above MSL.
 * @returns True if everything is good, false if there was an error.
 */
bool GravityComputer::Update(float lat_rad, float lon_rad, float alt_msl)
{
    // If we for whatever reason get a false, default to WGS84 gravity.
    if (!this->_ComputeGravity(lat_rad, lon_rad, alt_msl))
    {
        #ifdef DEBUG
        DEBUG_PRINTLN("GravityComputer::Update WARNING: Error encountered in gravity calculation. Defaulting to WGS84 gravity");
        #endif
        this->_grav = CONSTS_GRAV;
        this->errCount++;
        return false;
    }

    // Smooth out sharp changes
    this->_grav = GravSmoother.Filter(this->_grav);

    return true;
}


/**
 * Returns gravitational acceleration in [m/s/s].
 * 
 * @returns Gravitational acceleration in [m/s/s]
 * @see Update()
 * @warning Be sure to periodically call Update().
 */
float GravityComputer::GetGravity()
{
    return this->_grav;
}


/**
 * Compute gravitational acceleration in [m/s/s] using a gravity model
 * 
 * @param lat Geodetic latitude in [rad].
 * @param lon Geodetic longitude in [rad].
 * @param alt Altitude above MSL in [m].
 * @returns True if good calculation, false if an error was encountered.
 */
bool GravityComputer::_ComputeGravity(float lat, float lon, float alt)
{
    // Range check latitude
    // Lat. must be between -90deg and +90deg
    if (lat >= CONSTS_PIDIV2 || lat <= -CONSTS_PIDIV2)
    {
        #ifdef DEBUG
        DEBUG_PRINTLN("GravityComputer::_ComputeGravity ERROR: Latitude is out of bounds.");
        #endif
        return false;
    }

    // Range check longitude
    // Long. must be between -180deg and +180deg
    if (lon >= CONSTS_PI || lon <= -CONSTS_PI)
    {
        #ifdef DEBUG
        DEBUG_PRINTLN("GravityComputer::_ComputeGravity ERROR: Longitude is out of bounds.");
        #endif
        return false;
    }
        
    
    // Range check altitude
    // Alt. should be greater than -400m MSL (lowest point on earth) and 
    // less than +3400m above MSL (mean altitude of Nepal).
    // Meh, these should be good enough for sanity checks :P
    if (alt >= 3400.0f || alt <= -400.0f)
    {
        #ifdef DEBUG
        DEBUG_PRINTLN("GravityComputer::_ComputeGravity ERROR: Altitude is out of reasonable bounds for a quadcopter.");
        #endif
        return false;
    }

    #ifdef GRAV_COMPUTER_WGS84_MODEL
    // WGS84 gravity equation
    float sinLatSq = sinf(lat) * sinf(lat);
    this->_grav = 9.780327f * ((1.0f + (0.00193185138639f * sinLatSq) / sqrtf(1.0f - (0.006694379990141318f * sinLatSq)));  // Grav. at sea level
    #else
    // Helmert's equation
    float sinLat = sinf(lat);
    float sin2Lat = sinf(2.0f * lat);
    this->_grav = 9.780327f * (1.0f + (0.0053024f * sinLat * sinLat) - (0.0000058f * sin2Lat * sin2Lat));  // Grav. at sea level
    #endif

    // Account for altitude
    #ifdef GRAV_COMPUTER_NONLINEAR_FAC
    // Nonlinear free-air correction
    this->_grav += (3.986004418E14f / ((CONSTS_WGS84_A + alt) * (CONSTS_WGS84_A + alt))) - (3.986004418E14f / (CONSTS_WGS84_A * CONSTS_WGS84_A))
    // float denom = (1.0f + (alt / CONSTS_WGS84_A));
    // this->_grav = g0 / (denom * denom);
    #else
    // Linearized free-air correction (a good approx. for low altitudes)
    this->_grav -= 0.000003085f * alt;  // (GM * 2) / Re**3 = 0.000003072460023730221f 
    #endif

    // Range check computed gravity
    if (this->_grav >= 10.0f || this->_grav <= 9.5)
    {
        #ifdef DEBUG
        DEBUG_PRINTLN("GravityComputer::_ComputeGravity ERROR: Computed gravity is out of specified bounds.");
        #endif
        return false;
    }

    return true;
}


// GetInstance() function for singleton. Don't use as a user.
GravityComputer &GravityComputer::GetInstance()
{
    static GravityComputer instance;
    return instance;
}

GravityComputer &GravComputer = GravComputer.GetInstance();
