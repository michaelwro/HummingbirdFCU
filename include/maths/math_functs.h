// ----------------------------------------------------------------------------
// EXTRA MATH FUNCTIONS
// 
// Code By: Michael Wrona | B.S. Aerospace Engineering
// Created: 
// ----------------------------------------------------------------------------
/**
 * Extra math functions such as fast square root, 'safe' trig. functions, etc.
 */

#pragma once

#include <Arduino.h>
#include <math.h>
#include <float.h>
#include "Constants.h"
#include "hummingbird_config.h"


float InvSqrtf(float num);  // Used for normalizing

template <typename T>
T RangeConstrain(const T val, const T lower, const T upper);  // Constrain value to a range

template <typename T>
float sqrtf_safe(const T val);  // Check for computational errors in sqrt

template <typename T>
float asinf_safe(const T val);  // Take arcsine with checks

// template <typename T>
// float acosf_safe(const T val);  // Take arccosine with checks
