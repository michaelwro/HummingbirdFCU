// ----------------------------------------------------------------------------
// FLIGHT CONTROLLER GLOBAL CONSTANTS
// 
// Code By: Michael Wrona | B.S. Aerospace Engineering
// Created: 
// ----------------------------------------------------------------------------
/**
 * Global constants such as grav. accel, gas constants, air density, etc.
 * 
 * Resources
 * ~ Magnetic Field Calculator (NOAA):
 *     https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml#declination
 */

#pragma once


#include "hummingbird_config.h"


// ----------------------------------------------------------------------------
// Trigonometric constants
// ----------------------------------------------------------------------------
constexpr float CONSTS_PIDIV4   = 0.7853981634f;  // PI / 4
constexpr float CONSTS_PIDIV2   = 1.5707963268f;  // PI / 2
constexpr float CONSTS_PI       = 3.14159265359f;  // PI
constexpr float CONSTS_2PI      = 6.28318530718f;  // 2 * PI


// ----------------------------------------------------------------------------
// WGS84 Standard (Earth) Ellipsoid Parameters
// Parameters obtained from: Principles of GNSS, Inertial, and Multisensor 
// Integrated Navigation Systems by Paul D. Groves (2008).
// ----------------------------------------------------------------------------
constexpr float CONSTS_GRAV = 9.80665f;  // WGS84 gravitational acceleration [m/s/s]
constexpr float CONSTS_WGS84_A  = 6378137.0f;  // WGS84 earth semi-major axis [m]
constexpr float CONSTS_WGS84_B  = 6356752.3142f;  // WGS84 earth semi-minor axis [m]
constexpr float CONSTS_WGS84_E  = 0.0818191908425f;  // WGS84 earth eccentricity
constexpr float CONSTS_WGS84_INVF   = 298.257223563f;  // Inverse of WGS84 flattening, 1/f
// constexpr float CONSTS_WGS84_GRAVPARAM  = 3.986004418e+14F;  // WGS84 gearth ravity parameter (mu) [m^3/s^2]


// ----------------------------------------------------------------------------
// Environment constants
// ----------------------------------------------------------------------------
constexpr float CONSTS_ENV_AIRDENS  = 1.225f;  // Standard sea-level atmospheric air density [kg/m^3]
constexpr float CONSTS_ENV_TLAPSERATE  = 0.0065f;  // Standard atmospheric temperature lapse rate [C/m]
constexpr float CONSTS_ENV_SLPRES   = 101325.0f;  // Standard atmospheric air pressure at sea-level [Pa]
constexpr float CONSTS_ENV_SLTEMP   = 288.15f;  // Standard atmospheric air temperature at sea-level [K]
constexpr float CONSTS_ENV_R        = 287.26f;  // Gas constant for air [J/kg.K]


// ----------------------------------------------------------------------------
// Geomagnetig constants
// ----------------------------------------------------------------------------
constexpr float CONSTS_ENV_MAGDECL  = -0.000602662f;  // Local magnetic field declination [rad] (in degrees: -0.03453deg)
constexpr float CONSTS_ENV_MAGINCL  = 1.24428537f;  // Local magnetic field inclnation [rad] (in degrees: 71.2923deg)
