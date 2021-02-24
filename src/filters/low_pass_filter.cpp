// ----------------------------------------------------------------------------
// DISCRETE LOW PASS FILTER IMPLEMENTATION
// 
// Code By: Michael Wrona
// Created: 3 Feb 2021
// ----------------------------------------------------------------------------
/**
 * This is a simple implementation of a discrete low pass filter to filter
 * noisy signals. A low pass filter will let low-frequency signals through and
 * attenuate high-frequency signals (-3dB cutoff).
 */

#include <Arduino.h>
#include "filters/low_pass_filter.h"


// ------------------------------------
// Public methods
// ------------------------------------

// ----------------------------------------------------------------------------
// LowPassFilter()
// ----------------------------------------------------------------------------
/**
 * Constructor for the low pass filter library. Be sure to specify the 
 * smoothing factor!
 * 
 */
LowPassFilter::LowPassFilter()
{
    this->_setsf = false;
    this->_outPoint = 0.0f;
    this->_a = 1.0f;
}


// ----------------------------------------------------------------------------
// SetSmoothingFactor(float newSF)
// ----------------------------------------------------------------------------
/**
 * Set the filter's smoothing factor (alpha) between [0, 1]
 * 
 * @param newSF     Filter smoothing factor, [0, 1]
 */
void LowPassFilter::SetSmoothingFactor(float newSF)
{
    this->_a = RangeConstrain(newSF, 0.0f, 1.0f);
    this->_setsf = true;
}


// ----------------------------------------------------------------------------
// FilterWithSF(float rawPoint)
// ----------------------------------------------------------------------------
/**
 * Apply the LFP (defined by the smoothing factor) to a new point and output 
 * the filtered value.
 * 
 * @param rawPoint  Raw data point to filter
 * @return          Filtered measurement
 */
float LowPassFilter::FilterWithSF(float rawPoint)
{
    if (this->_setsf == false)  // Just return unfiltered measurement
    {
        this->_outPoint = rawPoint;
        return this->_outPoint;
    }

    this->_outPoint = (this->_a * rawPoint) + ((1.0f - this->_a) * this->_outPoint);
    return this->_outPoint;
}


// Low Pass Filter Deconstructor
LowPassFilter::~LowPassFilter() {}
