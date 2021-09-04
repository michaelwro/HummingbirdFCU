// ----------------------------------------------------------------------------
// INPUT VOLTAGE MONITOR
// 
// Code By: Michael Wrona
// Created: 4 Sept 2021
// ----------------------------------------------------------------------------
// Read the HFCU's voltage divider to read the input voltage on the Teensy 
// 4.1 Pin 22.

#include "sensor_systems/battery_monitor.h"


/**
 * Constructor for a voltage monitor. Initialize variables and filters.
 */
VoltageMonitor::VoltageMonitor()
    : Filter((size_t)5)
{
    int vRaw;

    // initialize filter with a single reading
    vRaw = analogRead((uint8_t)VCC_PIN);
    this->v = (BATTMONITOR_SF * (float)vRaw) + BATTMONITOR_OFFSET;
    this->Filter.Fill(this->v);
}


void VoltageMonitor::Update()
{
    int vRaw;

    // Read pin ADC
    vRaw = analogRead((uint8_t)VCC_PIN);

    // Calibration converts ADC [0, 1023] output to corrected voltage
    // By default, Teensy 4.1 has a 10-bit ADC
    this->v = (BATTMONITOR_SF * (float)vRaw) + BATTMONITOR_OFFSET;

    this->v = this->Filter.Filter(this->v);
}


float VoltageMonitor::GetVoltage()
{
    return this->v;
}






/* For singleton. Do not use this function as a user */
VoltageMonitor &VoltageMonitor::GetInstance()
{
    static VoltageMonitor instance;
    return instance;
}

VoltageMonitor &BattMonitor = BattMonitor.GetInstance();
