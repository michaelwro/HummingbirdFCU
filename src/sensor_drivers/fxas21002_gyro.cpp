// ----------------------------------------------------------------------------
// FXAS21002C GYROSCOPE SENSOR I2C DRIVER CODE
// 
// Code By: Michael Wrona
// Created: 25 July 2020
// Modified: 26 Aug 2021
// ----------------------------------------------------------------------------
// This is the sensor library for the FXAS21002C 3-axis gyroscope sensor in 
// I2C mode.
// Datasheet Specs:
//   16-bit ADC
//   +/- 250dps to 2000dps measurement ranges
//   Integrated LPF
//   8-bit temperature sensor
//   Temperature sensitivity of +/- 0.08 %/degC (max)
//   Nonlinearity of +/- 0.5 %FSR
//   0.025 dps/sqrt(Hz) noise density (FS=200Hz, CTRL_REG0[FS] = 00, CTRL_REG0[BW] = 00)


#include "sensor_drivers/fxas21002_gyro.h"


/**
 * FXAS21002 Constructor.
 * Constructor for the FXAS21002 gyro sensor.
 * 
 * @param wireInput  I2C/Wire interface the device is connected to.
 */
FXAS21002Gyro::FXAS21002Gyro(TwoWire *wireInput)
{
    // Clear raw data
    this->_gx = 0.0f;
    this->_gy = 0.0f;
    this->_gz = 0.0f;
    this->prevMeasMicros = micros();
    this->_SensorWire = wireInput;
}


/**
 * Initialize and configure gyroscope sensor.
 * If unspecified, the default gyro measurement rage is GYRO_RNG_1000DPS
 * 
 * @param rng Gyro measurement range.
 * @see GyroRanges_t
 * @return  True if successful, false if failed.
 */
bool FXAS21002Gyro::Initialize(GyroRanges_t rng)
{
    uint8_t ctrlReg0;
    uint8_t connectedSensorID;

    this->_SensorWire->begin();
    this->gyroRange = rng;  // Set range
    

    // Check to make sure the ID register on the sensor matches the expected FXAS21002C ID.
    connectedSensorID = this->I2Cread8(GYRO_REG_ID);
    if (connectedSensorID != FXAS21002C_ID)
    {
        #ifdef FXAS21002_DEBUG
        DEBUG_PRINTLN("FXAS21002::Initialize ERROR: Received gyro ID does not match expected.");
        #endif
        return false;
    }
    
    // Set sensor configuration parameters
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
            #ifdef FXAS21002_DEBUG
            DEBUG_PRINTLN("FXAS21002::Initialize ERROR: Unknown gyro range specified.");
            #endif
            return false;
            break;
    }

    
    // Reset sensor, then switch to active mode to configure.
    // EXAMPLE: setting GYRO_REG_CTRL0 to 0x01 and GYRO_REG_CTRL1 to 0x06 yields:
    //   FS = 400Hz
    //   FS range = +/- 1000dps
    //   HPF disabled
    //   LFP bandwidth = 128Hz

    this->I2Cwrite8(GYRO_REG_CTRL1, 0x00);  // Stby
    this->I2Cwrite8(GYRO_REG_CTRL1, (1 << 6));  // Reset
    this->I2Cwrite8(GYRO_REG_CTRL0, ctrlReg0);  // Set sensitivity
    this->I2Cwrite8(GYRO_REG_CTRL1, 0x06);  // Active, ODR = 400Hz
    // this->I2Cwrite8(GYRO_REG_CTRL1, 0x0C);  // Active, ODR = 800Hz
    delay(100);  // Short delay

    return true;
}


/**
 * Read gyroscope data from device registers.
 * Raw gyro readings are in [deg/s].
 * 
 * @return  True if successful, false if failed.
 */
bool FXAS21002Gyro::ReadSensor()
{
    int16_t gxRaw;
    int16_t gyRaw;
    int16_t gzRaw;

    // Read 7 bytes from sensor
    this->_SensorWire->beginTransmission((uint8_t)FXAS21002C_ADDRESS);
    this->_SensorWire->write(GYRO_REG_STATUS | 0x80);
    this->_SensorWire->endTransmission();
    this->_SensorWire->requestFrom((uint8_t)FXAS21002C_ADDRESS, (uint8_t)7);

    uint8_t status = this->_SensorWire->read();
    uint8_t xhi = this->_SensorWire->read();
    uint8_t xlo = this->_SensorWire->read();
    uint8_t yhi = this->_SensorWire->read();
    uint8_t ylo = this->_SensorWire->read();
    uint8_t zhi = this->_SensorWire->read();
    uint8_t zlo = this->_SensorWire->read();

    // Shift values to make proper integer, then cast int to float (with no decimals)
    gxRaw = (int16_t)((xhi << 8) | xlo);
    gyRaw = (int16_t)((yhi << 8) | ylo);
    gzRaw = (int16_t)((zhi << 8) | zlo);
    this->_gx = (float)gxRaw;  // units of LSB
    this->_gy = (float)gyRaw;
    this->_gz = (float)gzRaw;

    this->prevMeasMicros = micros();

    // Convert int readings to floats [dps] depending on sensitivity (deg/LSB)
    switch (this->gyroRange)
    {
        case GYRO_RNG_250DPS:
            this->_gx *= GYRO_SENS_250;
            this->_gy *= GYRO_SENS_250;
            this->_gz *= GYRO_SENS_250;
            break;
        case GYRO_RNG_500DPS:
            this->_gx *= GYRO_SENS_500;
            this->_gy *= GYRO_SENS_500;
            this->_gz *= GYRO_SENS_500;
            break;
        case GYRO_RNG_1000DPS:
            this->_gx *= GYRO_SENS_1000;
            this->_gy *= GYRO_SENS_1000;
            this->_gz *= GYRO_SENS_1000;
            break;
        case GYRO_RNG_2000DPS:
            this->_gx *= GYRO_SENS_2000;
            this->_gy *= GYRO_SENS_2000;
            this->_gz *= GYRO_SENS_2000;
            break;
        default:
            return false;  // TODO: Raise error?
            break;
    }

//    // Convert [dps] to [rad/s]
//    this->_gx *= 0.01745329251994329576923690768489f;
//    this->_gy *= 0.01745329251994329576923690768489f;
//    this->_gz *= 0.01745329251994329576923690768489f;

    return true;
}


/**
 * Read device's 8-bit temperature register and return in degrees C.
 * Temperature will not have any decimals, as it is an 8-bit signed int (-127C 
 * to 127C). Note that the temperature is not factory calibrated!
 * 
 * @returns Temperature in [C]
 */
float FXAS21002Gyro::GetTemperature()
{
    int8_t tempRead;
    float scaling;
    float tempC;

    scaling = 1.0f;  // [deg.C / LSB]
    tempRead = this->I2Cread8(GYRO_REG_TEMP);
    tempC = (float)tempRead * scaling;
    return tempC;
}


/**
 * Return gyro x-measurement in [deg/s]
 */
float FXAS21002Gyro::GetGx()
{
    return this->_gx;
}


/**
 * Return gyro y-measurement in [deg/s]
 */
float FXAS21002Gyro::GetGy()
{
    return this->_gy;
}


/**
 * Return gyro z-measurement in [deg/s]
 */
float FXAS21002Gyro::GetGz()
{
    return this->_gz;
}


/**
 * Write to FXAS21002 device register over I2C.
 * 
 * @param regOfInterest  Register address on device.
 * @param valToWrite     Value to write to register.
 */
void FXAS21002Gyro::I2Cwrite8(uint8_t regOfInterest, uint8_t valToWrite)
{
    // Init. communication
    this->_SensorWire->beginTransmission(FXAS21002C_ADDRESS);
    this->_SensorWire->write((uint8_t)regOfInterest);
    this->_SensorWire->write((uint8_t)valToWrite);
    this->_SensorWire->endTransmission();
}


/**
 * Read FXAS21002 register value over I2C.
 * 
 * @param regOfInterest Register address on device.
 * @return Value stored in the register.
 */
uint8_t FXAS21002Gyro::I2Cread8(uint8_t regOfInterest)
{
    uint8_t val;

    // Init. communication
    this->_SensorWire->beginTransmission((uint8_t)FXAS21002C_ADDRESS);
    this->_SensorWire->write((uint8_t)regOfInterest);

    // Check for failure
    if (this->_SensorWire->endTransmission(false) != 0)
        return 0;
    
    // Read register
    this->_SensorWire->requestFrom((uint8_t)FXAS21002C_ADDRESS, (uint8_t)1);
    val = this->_SensorWire->read();

    return val;
}
