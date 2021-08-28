// ----------------------------------------------------------------------------
// FLIGHT CONTROLLER CONVERSIONS
// 
// Code By: Michael Wrona | B.S. Aerospace Engineering
// Created: 
// ----------------------------------------------------------------------------
/**
 * Define conversion factors 'n stuff.
 */

#pragma once


#include "hummingbird_config.h"


constexpr float DEG2RAD  = 0.01745329251994329576923690768489f;  // PI / 180, convert degrees to radians
constexpr float RAD2DEG  = 57.295779513082320876798154814105f;  // 180 / PI, convert radians to degrees

// --------------------------------------
// Conversion factors and equations
// --------------------------------------
class Conversions
{
public:
    // --------------------------------------
    // Conversion factors and equations
    // --------------------------------------
    Conversions(){}  // Constructor
    
    /* Convert celsius to kelvin */
    float C2K(float inC) { return (inC + 273.15f); }

    /* Convert kelvin to celsius */
    float K2C(float inK) { return (inK - 273.15f); }

    /* Convert celsius to fahrenheit. */
    float C2F(float inC) { return ((inC * (9.0f / 5.0f)) + 32.0f); }

    /* Convert fahrenheit to celsius. */
    float F2C(float inF) { return ((inF - 32.0f) * (5.0f / 9.0f)); }

    /* Convert meters to feet */
    float Meters2Feet(float meters) { return (meters * 3.28084f); }
    
    /* Convert feet to meters */
    float Feet2Meters(float feet) { return (feet / 3.28084f); }

    /* Convert pressure in Pa to in.Hg */
    float Pascals2InHg(float pa) { return (pa * 0.0002953f); }

    /* Convert pressure in in.Hg to Pa */
    float InHg2Pascals(float inhg) { return (inhg * 3386.38673f); }
};
