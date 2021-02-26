// ----------------------------------------------------------------------------
// FXAS21002C GYROSCOPE SENSOR LIBRARY
// A custom sensor library for the FXAS21002C I2C gyroscope sensor. Inspired by
// Adafruit's FXAS21002C Library (see Resources).
// 
// Code By: Michael Wrona | B.S. Aerospace Engineering
// Created: 25 July 2020
// ----------------------------------------------------------------------------
/**
 * This is the sensor library for the FXAS21002C 3-axis gyroscope sensor. This
 * library was inspired by Adafruit's FXAS21002C Library (see Resources). Tested
 * and verified with Adafruit's FXAS21002C/FXOS8700 9-DOF IMU and an Arduino Uno.
 * 
 * Resources
 * ---------
 * ~ Adafruit FXAS21002C Sensor Library (GitHub):
 *     https://github.com/adafruit/Adafruit_FXAS21002C
 * ~ FXOX8700 + FXAS21002 9-DOF IMU (Adafruit):
 *     https://www.adafruit.com/product/3463
 */

#include "Arduino.h"
#include <Wire.h>
#include <limits.h>
#include "sensors/fxas21002_gyro.h"



// ------------------------------------
// Public methods
// ------------------------------------

// ----------------------------------------------------------------------------
// FXAS21002Gyro(TwoWire *wireInput)
// ----------------------------------------------------------------------------
/**
 * Constructor for FXAS21002C gyro sensor.
 * 
 * @param gyroID  ID for sensor, 0x0021002C
 */
FXAS21002Gyro::FXAS21002Gyro(TwoWire *wireInput)
{
    // Clear raw data
    this->gx = 0.0f;
    this->gy = 0.0f;
    this->gz = 0.0f;
    this->prevMeasMicros = micros();
    this->sensorI2C = wireInput;
}


// ----------------------------------------------------------------------------
// InitializeSensor(GyroRanges_t range)
// ----------------------------------------------------------------------------
/**
 * Initialize and configure gyroscope sensor. If unspecified, the default gyro
 * measurement rage is GYRO_RNG_1000DPS. Options include: GYRO_RNG_250DPS,
 * GYRO_RNG_500DPS, GYRO_RNG_1000DPS, and GYRO_RNG_2000DPS
 * 
 * @return  True if successful, false if failed.
 */
bool FXAS21002Gyro::Initialize(GyroRanges_t rng)
{
    uint8_t ctrlReg0;
    uint8_t connectedSensorID;

    this->sensorI2C->begin();
    this->gyroRange = rng;  // Set range
    

    /* Check to make sure the ID register on the sensor matches the
    expected FXAS21002C ID. */
    connectedSensorID = this->I2Cread8(GYRO_REG_ID);
    if (connectedSensorID != FXAS21002C_ID)
        return false;  // ID's dont match!
    
    /* Set sensor configuration parameters */
    ctrlReg0 = 0x00;
    switch (this->gyroRange)
    {
        case GYRO_RNG_250DPS:
            ctrlReg0 = 0x03;
            break;
        case GYRO_RNG_500DPS:
            ctrlReg0 = 0x02;
            break;
        case GYRO_RNG_1000DPS:
            ctrlReg0 = 0x01;
            break;
        case GYRO_RNG_2000DPS:
            ctrlReg0 = 0x00;
            break;
        default:
            return false;  // TODO: Raise error?
            break;
    }

    /* Reset sensor, then switch to active mode to configure */
    this->I2Cwrite8(GYRO_REG_CTRL1, 0x00);  // Stby
    this->I2Cwrite8(GYRO_REG_CTRL1, (1 << 6));  // Reset
    this->I2Cwrite8(GYRO_REG_CTRL0, ctrlReg0);  // Set sensitivity
    // this->I2Cwrite8(GYRO_REG_CTRL1, 0x0E);  // Active, ODR = 400Hz
    this->I2Cwrite8(GYRO_REG_CTRL1, 0x0C);  // Active, ODR = 800Hz
    delay(100);  // Short delay

    return true;
}


// ----------------------------------------------------------------------------
// ReadSensor()
// ----------------------------------------------------------------------------
/**
 * Read gyroscope data from device registers. Computes gyro readings in [rad/s]
 * 
 * @return  True if successful, false if failed.
 */
bool FXAS21002Gyro::ReadSensor()
{
    // Read 7 bytes from sensor
    this->sensorI2C->beginTransmission((uint8_t)FXAS21002C_ADDRESS);
    this->sensorI2C->write(GYRO_REG_STATUS | 0x80);
    this->sensorI2C->endTransmission();
    this->sensorI2C->requestFrom((uint8_t)FXAS21002C_ADDRESS, (uint8_t)7);

    uint8_t status = this->sensorI2C->read();
    uint8_t xhi = this->sensorI2C->read();
    uint8_t xlo = this->sensorI2C->read();
    uint8_t yhi = this->sensorI2C->read();
    uint8_t ylo = this->sensorI2C->read();
    uint8_t zhi = this->sensorI2C->read();
    uint8_t zlo = this->sensorI2C->read();

    // Shift values to make proper integer
    this->gx = (int16_t)((xhi << 8) | xlo);
    this->gy = (int16_t)((yhi << 8) | ylo);
    this->gz = (int16_t)((zhi << 8) | zlo);

    this->prevMeasMicros = micros();

    // Convert int readings to floats [dps] depending on sensitivity
    switch (this->gyroRange)
    {
        case GYRO_RNG_250DPS:
            this->gx *= GYRO_SENS_250;
            this->gy *= GYRO_SENS_250;
            this->gz *= GYRO_SENS_250;
            break;
        case GYRO_RNG_500DPS:
            this->gx *= GYRO_SENS_500;
            this->gy *= GYRO_SENS_500;
            this->gz *= GYRO_SENS_500;
            break;
        case GYRO_RNG_1000DPS:
            this->gx *= GYRO_SENS_1000;
            this->gy *= GYRO_SENS_1000;
            this->gz *= GYRO_SENS_1000;
            break;
        case GYRO_RNG_2000DPS:
            this->gx *= GYRO_SENS_2000;
            this->gy *= GYRO_SENS_2000;
            this->gz *= GYRO_SENS_2000;
            break;
        default:
            return false;  // TODO: Raise error?
            break;
    }

   // Convert [dps] to [rad/s]
   this->gx *= DEG2RAD;
   this->gy *= DEG2RAD;
   this->gz *= DEG2RAD;

    return true;
}


// ------------------------------------
// Private methods
// ------------------------------------

// ----------------------------------------------------------------------------
// I2Cwrite8(byte regOfInterest, byte valToWrite)
// ----------------------------------------------------------------------------
/**
 * Write to device register over I2C.
 * 
 * @param regOfInterest  Register address on device.
 * @param valToWrite     Value to write to register.
 */
void FXAS21002Gyro::I2Cwrite8(uint8_t regOfInterest, uint8_t valToWrite)
{
    // Init. communication
    this->sensorI2C->beginTransmission(FXAS21002C_ADDRESS);
    this->sensorI2C->write((uint8_t)regOfInterest);
    this->sensorI2C->write((uint8_t)valToWrite);
    this->sensorI2C->endTransmission();
}


// ----------------------------------------------------------------------------
// I2Cread8(byte regOfInterest)
// ----------------------------------------------------------------------------
/**
 * Read register value from I2C device.
 * 
 * @param regOfInterest  Register address on device.
 * @return               Value/data in register.
 */
uint8_t FXAS21002Gyro::I2Cread8(uint8_t regOfInterest)
{
    uint8_t val;

    // Init. communication
    this->sensorI2C->beginTransmission((uint8_t)FXAS21002C_ADDRESS);
    this->sensorI2C->write((uint8_t)regOfInterest);

    // Check for failure
    if (this->sensorI2C->endTransmission(false) != 0)
        return 0;
    
    // Read register
    this->sensorI2C->requestFrom((uint8_t)FXAS21002C_ADDRESS, (uint8_t)1);
    val = this->sensorI2C->read();

    return val;
}
