// ----------------------------------------------------------------------------
// FXAS21002 GYRO UNIT TESTS
//
// Code By: Michael Wrona
// Created: 27 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are unit test to make sure we can connect to the FXAS21002 gyro sensor 
 * and that it is working properly.
 */

#ifdef UNIT_TEST
#pragma once

#include <unity.h>
#include <Wire.h>
#include <math.h>
#include <Arduino.h>
#include "hummingbird_config.h"
#include "sensor_drivers/fxas21002_gyro.h"

/* Global gyro object for testing */
// FXAS21002Gyro gyro(&SENSOR_I2C);

void test_fxas21002_init_250dps(void);
void test_fxas21002_init_500dps(void);
void test_fxas21002_init_1000dps(void);
void test_fxas21002_init_2000dps(void);
void test_fxas21002_self_test(void);


void FXAS21002_I2Cwrite8(TwoWire *sensorWire, uint8_t regOfInterest, uint8_t valToWrite);
uint8_t FXAS21002_I2Cread8(TwoWire *sensorWire, uint8_t regOfInterest);





#endif  // UNIT_TEST
