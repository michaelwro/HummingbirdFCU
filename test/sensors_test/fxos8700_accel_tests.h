// ----------------------------------------------------------------------------
// FXOS8700 ACCELEROMETER UNIT TESTS
// 
// Code By: Michael Wrona
// Created: 27 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit tests to test the functionality and health of the FXOS8700 
 * accelerometer sensor. These make sure we can connect to the sensor and if it 
 * can be configured properly. This also puts the sensor into self-test mode to 
 * verify the signal chain functionality (p.8 in datasheet).
 */

#ifdef UNIT_TEST
#pragma once
#include <unity.h>
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "hummingbird_config.h"
#include "sensor_drivers/fxos8700_accelmag.h"


void test_fxos8700_init_2g(void);
void test_fxos8700_init_4g(void);
void test_fxos8700_init_8g(void);
void test_fxos8700_self_test(void);

void FXOS8700_I2Cwrite8(TwoWire *sensorWire, uint8_t regOfInterest, uint8_t valToWrite);
uint8_t FXOS8700_I2Cread8(TwoWire *sensorWire, uint8_t regOfInterest);



#endif  // UNIT_TEST
 