// ----------------------------------------------------------------------------
// DEBUGGING TOOL DEFINITIONS
// 
// Code By: Michael Wrona
// Created: 28 Aug 2021
// ----------------------------------------------------------------------------
/**
 * These are debugging variables and macros used to print debugging 
 * information to the serial terminal.
 */


#pragma once
#include <Arduino.h>
#include "hummingbird_config.h"


/**
 * ====================================
 * ENABLE/DISABLE DEBUG MESSAGES
 * ====================================
 * ~~ SUPRESS DEBUG MESSAGES BEFORE FLIGHT! ~~
 * Uncomment to print debug statements. Comment out to supress messages. 
 * Found in debugging.h
 */
#define DEBUG

/**
 * ====================================
 * DEBUG SERIAL/UART INTERFACE
 * ====================================
 * Use the Teensy 4.1's primary serial interface for printing debug messages. 
 * Found in debugging.h
 */
#define DEBUG_PORT Serial

/**
 * ====================================
 * DEBUG PRINTS
 * ====================================
 * Macros to print messages to the debug port. The F() macro stores messages 
 * in flash memory instead of RAM, which should help with storage.
 */
#ifdef DEBUG
    #define DEBUG_PRINT(msg) DEBUG_PORT.print(F(msg))  // Print message to debug terminal
    #define DEBUG_PRINTF(val, digits) DEBUG_PORT.print(val, digits)  // Print float value to debug terminal
    #define DEBUG_PRINTLN(msg) DEBUG_PORT.println(F(msg))  // Print message to debug terminal with newline
    #define DEBUG_PRINTLNF(val, digits) DEBUG_PORT.println(val, digits)  // Print float to debug terminal with newline
#else
    #define DEBUG_PRINT(msg)  // Print message to debug terminal. Blank macro because debuggng is disabled.
    #define DEBUG_PRINTF(val, digits)  // Print float value to debug terminal. Blank macro because debuggng is disabled.
    #define DEBUG_PRINTLN(msg)  // Print message to debug terminal with newline. Blank macro because debuggng is disabled.
    #define DEBUG_PRINTLNF(val, digits)  // Print float to debug terminal with newline. Blank macro because debuggng is disabled.
#endif
