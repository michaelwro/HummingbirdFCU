// ----------------------------------------------------------------------------
// EXTRA MATH FUNCTIONS
// 
// Code By: Michael Wrona | B.S. Aerospace Engineering
// Created: 
// ----------------------------------------------------------------------------
/**
 * Extra math functions such as fast square root, 'safe' trig. functions, etc.
 */

#include <Arduino.h>
#include "maths/math_functs.h"


// ----------------------------------------------------------------------------
// InvSqrtf(float num)
// ----------------------------------------------------------------------------
/**
 * norm = a * InvSqrtf(a);  Fast inverse square root algorithm. Use when 
 * normalizing vectors! See: 
 * https://en.wikipedia.org/wiki/Fast_inverse_square_root
 * 
 * 
 * @param num  Number to square-root.
 * @return  Inverse of square root.
 */
float InvSqrtf(float num)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5f;

    x2 = num * 0.5f;
    y = num;
    i = *(long*) &y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*) &i;
    y = y * (threehalfs - (x2 * y * y));
    return y;
}


// ----------------------------------------------------------------------------
// RangeConstrain(T val, T lower, T upper)
// ----------------------------------------------------------------------------
/**
 * Templates to constrain a value to a certain range. Works for floats, uint16,
 * etc. If value is out of specified bounds, constrain it to be within the
 * bounds.
 * 
 * @param val  Input value to constrain
 * @param lower  Lower bound
 * @param upper  Upper bound
 */
template <typename T>
T RangeConstrain(const T val, const T lower, const T upper) {
    // Check if input is NaN
    // From ardupilot firmware:
    // https://github.com/ArduPilot/ardupilot/blob/00cfc1932fe98452ede016ea9f9f799d10ea9fb8/libraries/AP_Math/AP_Math.cpp#L287
    if (std::is_floating_point<T>::value)
    {
        if (isnan(val))
        {
            return ((lower + upper) / 2);  // TODO: Throw error
        }
    }

    if (val <= lower)
        return lower;
    
    if (val >= upper)
        return upper;
    
    return val;
}
template int RangeConstrain<int>(int val, int lower, int upper);
template uint16_t RangeConstrain<uint16_t>(uint16_t val, uint16_t lower, uint16_t upper);
template float RangeConstrain<float>(const float val, const float lower, const float upper);


// ----------------------------------------------------------------------------
// sqrtf_safe(const T val)
// ----------------------------------------------------------------------------
/**
 * Compute safe square root of a value. Checks for NaN's
 * 
 * @param val  Value to take square root of.
 * @return  Square root of result.
 */
template <typename T>
float sqrtf_safe(const T val)
{
    float rez = sqrtf(static_cast<float>(val));
    if (isnan(rez))
        return 0;  // TODO: Raise error?
    
    return rez; 
}

template float sqrtf_safe<int>(const int val);
template float sqrtf_safe<uint16_t>(const uint16_t val);
template float sqrtf_safe<float>(const float val);


// ----------------------------------------------------------------------------
// asinf_safe(const T val)
// ----------------------------------------------------------------------------
/**
 * Take the arcsine of a number with safety checks. Return float
 * 
 * @param val  Value to take the arcsine of
 * @return  (float) Computed arcsine.
 */
template <typename T>
float asinf_safe(const T val)
{
    const float rez = static_cast<const float>(val);  // Cast to float

    if (isnan(rez))
        return 0.0f;  // TODO: Raise error
    
    if (rez >= 1.0f)
        return static_cast<float>(CONSTS_PIDIV2);
    
    if (rez <= -1.0f)
        return static_cast<float>(-CONSTS_PIDIV2);
    
    return asinf(val);
}

template float asinf_safe<int>(const int val);
template float asinf_safe<uint16_t>(const uint16_t val);
template float asinf_safe<float>(const float val);


// ----------------------------------------------------------------------------
// acos_safe(const T val)
// ----------------------------------------------------------------------------
/**
 * Take the arccosine of a number with safety checks. Return float
 * 
 * @param val  Value to take the arccosine of
 * @return  (float) Computed arccosine.
 */
// template <typename T>
// float acosf_safe(const T val)
// {
//     const float rez = static_cast<const float>(val);  // Cast to float

//     if (isnan(rez))
//         return 0.0f;  // TODO: Raise error
    
//     if (rez >= 1.0f)
//         return 0.0f;
    
//     if (rez <= -1.0f)
//         return CONSTS_PI;
    
//     return acosf(val);
// }

// template float acosf_safe<int>(const int val);
// template float acosf_safe<uint16_t>(const uint16_t val);
// template float acosf_safe<float>(const float val);

