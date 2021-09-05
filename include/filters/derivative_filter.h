// ----------------------------------------------------------------------------
// DERIVATIVE FILTER FROM "SMOOTH NOISE-ROBUST DIFFERENTIATORS"
// 
// Code By: Michael Wrona
// Created: 4 Sept 2021
// ----------------------------------------------------------------------------
// This is a derivative filter implementation used by Ardupilot to compute 
// vertical speed from barometric pressure readings. The original description 
// of this method can be found below. Ardupilot uses a 7-point filter for their
// vertical speed calculations and obviously works, so I went with that, too.
//
// https://github.com/ArduPilot/ardupilot/blob/c83774a7cd83929b90c772beec268d0fc1bf94d2/libraries/Filter/DerivativeFilter.cpp
// http://www.holoborodko.com/pavel/numerical-methods/numerical-derivative/smooth-low-noise-differentiators/

#pragma once
#include <Arduino.h>
#include <math.h>
#include "hummingbird_config.h"



#define DERIV_FILTER_INDEX(array, sample_index, i) array[(((sample_index-1)+i+1)+3*7/2) % 7]  ///< Access f(x) elements similar to the Derivative Smoother article: http://www.holoborodko.com/pavel/numerical-methods/numerical-derivative/smooth-low-noise-differentiators/
// #define DERIV_FILTER_INDEX_X(array, sample_index, i) array[(((sample_index-1)+i+1)+3*7/2) % 7]  ///< Access x elements similar to the Derivative Smoother article: http://www.holoborodko.com/pavel/numerical-methods/numerical-derivative/smooth-low-noise-differentiators/


class DerivativeFilter
{
public:
    DerivativeFilter();
    ~DerivativeFilter()
    {
        delete this->samples;
        delete this->timestamps;
    };

    float Filter(float val, uint32_t timestamp);

protected:
private:
    size_t sampleIdx;  ///< Index within buffer
    uint32_t *timestamps;  ///< [us] Buffer array to store previous micros() timestamps, accouns for non-uniform time samples
    float prevDeriv;  ///< Previously computed derivative
    float *samples;  ///< Buffer array to store previous values
};


