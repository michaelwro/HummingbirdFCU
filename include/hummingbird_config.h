// ----------------------------------------------------------------------------
// HUMMINGBIRD FCU CONFIGURATION PARAMETERS
// 
// Code By: Michael Wrona
// Created: 6 Feb 2021
// ----------------------------------------------------------------------------
/**
 * UBER-EXTREME CAUTION SHOULD BE USED CHANGING PARAMETERS IN THIS FILE. THEY  
 * AREN'T MEANT TO EVER CHANGE!
 * 
 * This header contains configuration parameters and a few #defines for the 
 * Hummingbird FCU.
 */

#pragma once

/**
 * ====================================
 * ENABLE/DISABLE DEBUG MESSAGES
 * ====================================
 * ~~ SUPRESS DEBUG MESSAGES BEFORE FLIGHT! ~~
 * Uncomment to print debug statements. Comment out to supress messages. 
 * Found in hummingbird_config.h
 */
#define DEBUG

/**
 * ====================================
 * DEBUG INTERFACE
 * ====================================
 * Use the Teensy 4.1's primary serial interface for printing debug messages. 
 * Found in hummingbird_config.h
 */
#define DebugPort Serial

/**
 * ====================================
 * SENSOR I2C BUS
 * ====================================
 * This is the Teensy 4.1 's I2C bus that the FXOS8700 accelerometer/compass, 
 * FXAS21002 gyro, GPS compass, and BMP388 barometer/temperature sensor are 
 * connected to. Found in hummingbird_config.h
 */
#define SensorI2C Wire2

/**
 * ====================================
 * GPS SERIAL/UART BUS
 * ====================================
 * UART/serial port that the GPS is connected to. Found in hummingbird_config.h
 */
#define GPSUart Serial7

/**
 * ====================================
 * TELEMETRY SERIAL/UART BUS
 * ====================================
 * UART/serial port that the XBee radio is connected to. Found in 
 * hummingbird_config.h
 */
#define TelemUart Serial8

/**
 * ====================================
 * AUXILLIARY I2C BUS
 * ====================================
 * Extra I2C bus on the top of the Hummingbird FCU. Used to connect extra I2C 
 * devices to the board. Found in hummingbird_config.h
 */
#define AuxI2C Wire

/**
 * ====================================
 * AUXILLIARY SERIAL/UART BUS
 * ====================================
 * UART/serial port used to connect an additional serial device. Found in 
 * hummingbird_config.h
 */
#define AuxUart Serial4

/**
 * ====================================
 * RGB LED PIN DEFINITIONS
 * ====================================
 * Found in hummingbird_config.h
 */
#define RED_LED 15          // Red status LED PWM
#define GRN_LED 14          // Green status LED PWM
// #define BLU_LED 13          // Blue status LED PWM

/**
 * Define floating point precision for floats close to zero. These parameters 
 * aretypically used to check if a number is close to zero to prevent divide 
 * by zero errors. Found in hummingbird_config.h
 */
constexpr float FLOAT_PREC_ZERO         = 1e-7F;  // Single floating point precision
constexpr float FLOAT_PREC_ZERO_NOISY   = 1e-5F;  // Single floating point precision used when 'kinda close to zero' is good enough
