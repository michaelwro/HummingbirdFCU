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

#pragma once

#include <Arduino.h>
#include "constants.h"
#include "maths/math_functs.h"
#include "hummingbird_config.h"


// ------------------------------------
// Low Pass Filter Class
// ------------------------------------
class LowPassFilter
{
public:
    LowPassFilter();
    ~LowPassFilter();
    void SetSmoothingFactor(float newSF = 1.0f);
    float FilterWithSF(float rawPoint);
    float dt;
protected:
private:
    bool _setsf;  // Set smoothing factor?
    float _outPoint;  // Previously filtered point
    float _a;  // Filter smoothing factor, [0, 1]
};
