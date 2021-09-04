// ----------------------------------------------------------------------------
// INPUT VOLTAGE MONITOR
// 
// Code By: Michael Wrona
// Created: 4 Sept 2021
// ----------------------------------------------------------------------------
// Read the HFCU's voltage divider to read the input voltage on the Teensy 
// 4.1 Pin 22.


#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "hummingbird_config.h"
#include "debugging.h"
#include "filters/median_filter.h"


// input_voltage = (SF * adc_output) + OFFSET
constexpr float BATTMONITOR_OFFSET  = 0.343256f; ///< Offset factor used to calibrate voltage measurements.
constexpr float BATTMONITOR_SF      = 0.016926f;  ///< Scale factor used to calibrate voltage measurements.




class VoltageMonitor
{
public:
    VoltageMonitor();
    ~VoltageMonitor() {};

    // Do not allow copies (singleton)
    static VoltageMonitor &GetInstance();  // accessor
    VoltageMonitor(const VoltageMonitor &) = delete;
    VoltageMonitor &operator=(const VoltageMonitor & ) = delete;

    void Update();
    float GetVoltage();

    uint32_t prevMeasMillis;  ///< Previous measurement time in millis()

protected:
private:
    float v;  ///< Measured voltage [volts]
    MedianFilter Filter;  ///< LPF to smooth out voltage
};


// Only one instance
extern VoltageMonitor &BattMonitor;


