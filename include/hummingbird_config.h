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
 * SENSOR I2C BUS
 * ====================================
 * This is the Teensy 4.1 's I2C bus that the FXOS8700 accelerometer/compass, 
 * FXAS21002 gyro, GPS compass, and BMP388 barometer/temperature sensor are 
 * connected to. Found in hummingbird_config.h
 */
#define SENSOR_I2C Wire2

/**
 * ====================================
 * GPS SERIAL/UART BUS
 * ====================================
 * UART/serial port that the GPS is connected to. Found in hummingbird_config.h
 */
// #define GPS_PORT Serial7

/**
 * ====================================
 * GPS I2C BUS
 * ====================================
 * I2C bus that the GPS is connected to. Found in hummingbird_config.h
 */
#define GPS_I2C Wire2

/**
 * ====================================
 * TELEMETRY SERIAL/UART BUS
 * ====================================
 * UART/serial port that the XBee radio is connected to. Found in 
 * hummingbird_config.h
 */
#define TELEM_PORT Serial8

/**
 * ====================================
 * AUXILLIARY I2C BUS
 * ====================================
 * Extra I2C bus on the top of the Hummingbird FCU. Used to connect extra I2C 
 * devices to the board. Found in hummingbird_config.h
 */
#define AUX_I2C Wire

/**
 * ====================================
 * AUXILLIARY SERIAL/UART BUS
 * ====================================
 * UART/serial port used to connect an additional serial device. Found in 
 * hummingbird_config.h
 */
#define AUX_SERIAL Serial4

/**
 * ====================================
 * VCC INPUT VOLTAGE ANALOG PIN
 * ====================================
 * This is the pin that is connected to the HFCU's voltage divider circuit to 
 * read input voltage.
 */
#define VCC_PIN 22

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
