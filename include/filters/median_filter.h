// ----------------------------------------------------------------------------
// MEDAIN FILTER
//
// Author: Michael Wrona
// Created: 08 July 2020
// ----------------------------------------------------------------------------
/**
 * A simple median filter implementation. Used to smooth noisy signals.
 */

#pragma once

#include <Arduino.h>
#include "hummingbird_config.h"

constexpr size_t MEDAINFILT_MAX_POINTS = 20;  // Max. number of points to store for filter

class MedianFilter
{
public:
    MedianFilter(size_t windowWidth = 3);
    MedianFilter(size_t windowWidth, float initVal);
    ~MedianFilter();
    void Fill(float val);
    size_t GetWindowWidth();
    float Filter(float newPoint);
    // float* GetWindow();
    
private:
    size_t N;
    size_t insertIndex;
    float *dataPoints;  // Pointer to the array, created with 'new'
};
