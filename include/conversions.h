// ----------------------------------------------------------------------------
// FLIGHT CONTROLLER CONVERSIONS
// 
// Code By: Michael Wrona | B.S. Aerospace Engineering
// Created: 
// ----------------------------------------------------------------------------
/**
 * Define conversion factors 'n stuff.
 */



#ifndef __CONVERSIONS_H__
#define __CONVERSIONS_H__


#include "hummingbird_config.h"


// ----------------------------------------------------------------------------
// Conversion factors and equations
//
// RAD_TO_DEG defined in Arduino.h, pi/180
// DEG_TO_RAD defined in Arduino.h, 180/pi
// ----------------------------------------------------------------------------
class Conversions
{
    public:
        Conversions(){}  // Constructor

        // /* Convert radians to degrees. */
        // float Rad2Deg(float inRad) { return inRad * (180.0f / CONSTS_PI); }

        // /* Convert degrees to radians. */
        // float Deg2Rad(float inDeg) { return inDeg * (CONSTS_PI / 180.0f); }
        
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

// Conversions Convert = Conversions();  // Create 'global' instance

#endif  // __CONVERSIONS_H__

