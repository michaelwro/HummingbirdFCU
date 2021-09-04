// ----------------------------------------------------------------------------
// MEDAIN FILTER
//
// Author: Michael Wrona
// Created: 08 July 2020
// ----------------------------------------------------------------------------
/**
 * A simple median filter implementation. Used to smooth noisy signals.
 */

#include <Arduino.h>
#include "filters/median_filter.h"

// ------------------------------------
// Public methods
// ------------------------------------

// ----------------------------------------------------------------------------
// MedianFilter(uint8_t windowWidth)
// ----------------------------------------------------------------------------
/**
 * Constructs the median filter class. Specify the window size upon class
 * initialization.
 * 
 * @param windowWidth  Number of elements to use to compute the median, 
 *                     number of past data points to store.
 */
MedianFilter::MedianFilter(size_t windowWidth)
{
    // Update window size (N) if input is given
    this->N = (windowWidth < MEDAINFILT_MAX_POINTS) ? windowWidth : MEDAINFILT_MAX_POINTS;
    this->insertIndex = 0;  // Begin at zero
    
    this->dataPoints = new float[this->N];  // Dynamic array on da heap

    // Set data array to zeros
    for (size_t i = 0; i < this->N; i++)
        this->dataPoints[i] = 0.0f;
}


// ----------------------------------------------------------------------------
// MedianFilter(size_t windowWidth, float initVal)
// ----------------------------------------------------------------------------
/**
 * Constructs the median filter class. Specify the window size upon class
 * initialization.
 * 
 * @param windowWidth  Number of elements to use to compute the median, 
 *                     number of past data points to store.
 * @param initVal  Initial value to fill the median filter array with.
 */
MedianFilter::MedianFilter(size_t windowWidth, float initVal)
{
    // Update window size (N) if input is given
    this->N = (windowWidth < MEDAINFILT_MAX_POINTS) ? windowWidth : MEDAINFILT_MAX_POINTS;
    this->insertIndex = 0;  // Begin at zero
    
    this->dataPoints = new float[this->N];  // Dynamic array on da heap

    // Set data array to zeros
    for (size_t i = 0; i < this->N; i++)
        this->dataPoints[i] = initVal;
}


// ----------------------------------------------------------------------------
// Fill(float val)
// ----------------------------------------------------------------------------
/**
 * Fill median filter array with a certain value. Use this to initialize the 
 * filter!
 * 
 * @param val  Value to fill the array with.
 */
void MedianFilter::Fill(float val)
{
    size_t i;

    for (i = 0; i < this->N; i++)
        this->dataPoints[i] = val;
}


// ----------------------------------------------------------------------------
// Filter(float newPoint)
// ----------------------------------------------------------------------------
/**
 * Add point to median filter and return filtered value.
 * 
 * @param newPoint  Noisy point to filter.
 * @return          Median-filtered value.
 */
float MedianFilter::Filter(float newPoint)
{
    float filtValue;  // Filtered/smoothed value to output
    
    // Insert new value into window, update window index
    this->dataPoints[this->insertIndex] = newPoint;
    this->insertIndex++;

    // Check to reset
    if (this->insertIndex >= this->N)
        this->insertIndex = 0;
    

    // Compute mean/average of values in window
    filtValue = 0.0f;
    for (size_t i = 0; i < this->N; i++)
        filtValue += this->dataPoints[i];
    
    return (filtValue / (float)this->N);
}


// ----------------------------------------------------------------------------
// GetWindowWidth()
// ----------------------------------------------------------------------------
/**
 * Return median filter window width.
 * @return  Median filter window width.
 */
size_t MedianFilter::GetWindowWidth()
{
    return this->N;
}


// ----------------------------------------------------------------------------
// GetWindow()
// ----------------------------------------------------------------------------
/**
 * Return pointer to median filter past values/window.
 * @return  Pointer to median filter past values/window.
 */
// float* MedianFilter::GetWindow()
// {
//     return this->dataPoints;
// }


// Median Filter Deconstructor
MedianFilter::~MedianFilter()
{
    delete[] this->dataPoints;
}
