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
// GravityComputer(float latRecomputeThres = 0.0005f, 
//                float altRecomputeThres = 5.0f)
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
class GravityComputer
{
public:
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
    GravityComputer(float latRecomputeThres = 0.0005f, float altRecomputeThres = 5.0f);
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
    float _altThres;  // [m] If altitude change is greater than this, recompute.
    float _latThres;  // [rad] If latitude change is greater than this, recompute.
    float _grav;  // Computed gravitational acceleration in [m/s/s]
    float _prevLat;  // Previously used latitude [rad]
    float _prevAlt;  // Previously used altitude msl [m]
};

// Only one instance of InertialNavSystem
extern GravityComputer &GravComputer;

