// ----------------------------------------------------------------------------
// DERIVATIVE FILTER FROM "SMOOTH NOISE-ROBUST DIFFERENTIATORS"
// 
// Code By: Michael Wrona
// Created: 4 Sept 2021
// ----------------------------------------------------------------------------
// This is a derivative filter implementation used by Ardupilot to compute 
// vertical speed from barometric pressure readings. The original description 
// of this method can be found below. Ardupilot uses a 7-point filter for their
// vertical speed calculations and obviously works well, so I went with that, too.
//
// https://github.com/ArduPilot/ardupilot/blob/c83774a7cd83929b90c772beec268d0fc1bf94d2/libraries/Filter/DerivativeFilter.cpp
// http://www.holoborodko.com/pavel/numerical-methods/numerical-derivative/smooth-low-noise-differentiators/


#include "filters/derivative_filter.h"


DerivativeFilter::DerivativeFilter()
{
    size_t i;
    // create and fill arrays
    this->samples = new float[7];
    this->sampleIdx = 0;
    for (i = 0; i < 7; i++)
        this->samples[i] = 0.0f;
    
    this->timestamps = new uint32_t[7];
    for (i = 0; i < 7; i++)
        this->timestamps[i] = 0;
    
    this->prevDeriv = 0.0f;  // Assume no slope initially
}


/**
 * Apply 7-point derivative filter to a new datapoint and return the filtered 
 * derivative of the input. Borrowed from Ardupilot.
 * 
 * @param val  New/raw value to filter and compute the derivative of.
 * @param timestamp  micros() timestamp of the observation. Used to compute 
 * step size between samples and deal with non-uniform samples.
 * @returns  Filtered derivative of the input.
 * @see https://github.com/ArduPilot/ardupilot/blob/c83774a7cd83929b90c772beec268d0fc1bf94d2/libraries/Filter/DerivativeFilter.cpp
 */
float DerivativeFilter::Filter(float val, uint32_t timestamp)
{
    size_t i;
    float derivative;

    // DerivativeFilter::update
    // If the current and prev. timestamps are equal, skip and return the previous slope.
    if (this->sampleIdx == 0)
    {
        i = 6;
    }
    else
    {
        i = this->sampleIdx - 1;
    }
    
    if (this->timestamps[i] == timestamp)
    {
        return this->prevDeriv;  // return prev. derivative value
    }

    // FilterWithBuffer::apply
    // Add value and timestamp to their arrays
    this->timestamps[this->sampleIdx] = timestamp;
    this->samples[this->sampleIdx++] = val;

    // wrap index if necessary
    if (this->sampleIdx >= 7)
    {
        this->sampleIdx = 0;
    }

    // DerivativeFilter::slope
    // Check if we have filled the buffer yet. If not, we cannot compute the derivative! Return zero.
    if (this->timestamps[6] == this->timestamps[5])
    {
        return 0.0f;
    }

    
    // compute the derivative
    // macro modified from Ardupilot's source code in DerivativeFilter.cpp
    derivative = 0.0f;
    derivative = 2.0f * 5.0f * (DERIV_FILTER_INDEX(this->samples, this->sampleIdx, 1) - DERIV_FILTER_INDEX(this->samples, this->sampleIdx, -1)) / ((float)DERIV_FILTER_INDEX(this->timestamps, this->sampleIdx, 1) - (float)DERIV_FILTER_INDEX(this->timestamps, this->sampleIdx, -1))  // cast from uint32_t to float
                + 4.0f * 4.0f * (DERIV_FILTER_INDEX(this->samples, this->sampleIdx, 2) - DERIV_FILTER_INDEX(this->samples, this->sampleIdx, -2)) / ((float)DERIV_FILTER_INDEX(this->timestamps, this->sampleIdx, 2) - (float)DERIV_FILTER_INDEX(this->timestamps, this->sampleIdx, -2))
                + 6.0f * 1.0f * (DERIV_FILTER_INDEX(this->samples, this->sampleIdx, 3) - DERIV_FILTER_INDEX(this->samples, this->sampleIdx, -3)) / ((float)DERIV_FILTER_INDEX(this->timestamps, this->sampleIdx, 3) - (float)DERIV_FILTER_INDEX(this->timestamps, this->sampleIdx, -3));
    derivative /= 32.0f;

    // check for numerical errors
    if (isnan(derivative) || isinf(derivative))
    {
        derivative = 0.0f;
    }
    
    // replace last result
    this->prevDeriv = derivative;

    return derivative;
}

