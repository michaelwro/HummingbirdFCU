// ----------------------------------------------------------------------------
// MAGNETOMETER/COMAPSS OBJECT
// 
// Code By: Michael Wrona
// Created : 4 Mar 2021
// ----------------------------------------------------------------------------
/**
 * The compass class is in charge of taking magnetometer measurements and 
 * computing tilt-compensated (magnetic) heading 
 */


#include "sensor_systems/compass.h"


// ----------------------------------------------------------------------------
// MagCompass(TwoWire *userWire)
// : Mag(3), MagRaw(3), MagSensor(&SENSOR_I2C)
// ----------------------------------------------------------------------------
/**
 * Constructor for the compass class. Be sure to specify the I2C bus that the 
 * sensor is connected to.
 * 
 * @param userWire   I2C bus the compass is connected to. Default Wire2
 */
MagCompass::MagCompass(TwoWire *userWire)
: Mag(3), MagRaw(3), MagSensor(&SENSOR_I2C)
{
    heading = 0.0f;
    prevUpdateMicros = micros();
}


// ----------------------------------------------------------------------------
// Initialize()
// ----------------------------------------------------------------------------
/**
 * Initialize the compass. Configures magnetometer sensor.
 * 
 * @returns True is success, false if not.
 */
bool MagCompass::Initialize()
{
    #ifdef MAGCOMPASS_DEBUG
    DEBUG_PORT.print("MAGCOMPASS:Initialize: Connecting to sensor... ");
    #endif

    /* Connect to and init. the magnetometer */
    if (!MagSensor.Initialize(MAGCOMPASS_RANGE))
    {
        #ifdef MAGCOMPASS_DEBUG
        DEBUG_PORT.println("MAGCOMPASS:Initialize ERROR: Could not initialize/connect to LIS3MDL compass. Check settings.");
        #endif
        return false;
    }

    #ifdef MAGCOMPASS_DEBUG
    DEBUG_PORT.println("Done!");
    #endif

    return true;
}


// ----------------------------------------------------------------------------
// Update()
// ----------------------------------------------------------------------------
/**
 * Update the compass. Record magnetometer data, rotate sensor data to the body 
 * frame, and apply calibration.
 * 
 * @returns True if successful, false if not.
 */
bool MagCompass::Update()
{
    float mx, my, mz;
    float bmx, bmy, bmz;

    /* Read sensor */
    if (!MagSensor.ReadSensor())
    {
        #ifdef MAGCOMPASS_DEBUG
        DEBUG_PORT.println("MAGCOMPASS:Update ERROR: Could not read magnetometer sensor.");
        #endif
        // return false;
    }

    // Apply sensor rotation
    #ifdef MAGCOMPASS_DO_NOT_ROTATE
    mx = MagSensor.GetMx();
    my = MagSensor.GetMy();
    mz = MagSensor.GetMz();
    #else
    mx = -MagSensor.GetMy();
    my = -MagSensor.GetMx();
    mz = -MagSensor.GetMz();
    #endif

    MagRaw.vec[0] = mx;
    MagRaw.vec[1] = my;
    MagRaw.vec[2] = mz;

    prevUpdateMicros = micros();

    /* Apply calibration */
    bmx = mx - SENSCALIB_MAG_BX;
    bmy = my - SENSCALIB_MAG_BY;
    bmz = mz - SENSCALIB_MAG_BZ;

    Mag.vec[0] = (SENSCALIB_MAG_S11 * bmx) + (SENSCALIB_MAG_S12 * bmy) + (SENSCALIB_MAG_S13 * bmz);
    Mag.vec[1] = (SENSCALIB_MAG_S12 * bmx) + (SENSCALIB_MAG_S22 * bmy) + (SENSCALIB_MAG_S23 * bmz);
    Mag.vec[2] = (SENSCALIB_MAG_S13 * bmx) + (SENSCALIB_MAG_S23 * bmy) + (SENSCALIB_MAG_S33 * bmz);

    return true;
}


// ----------------------------------------------------------------------------
// GetHeading(Vectorf AccelMeas)
// ----------------------------------------------------------------------------
/**
 * Compute and return tilt-compensated magnetic heading based on accelerometer 
 * data. Tilt-compensation equations can be found in: 
 * https://www.cypress.com/file/130456/download
 * 
 * @param AccelMeas [m/s/s] Vectorf of accelerometer measurements
 * @returns [rad] Tilt-compensated heading
 */
float MagCompass::GetHeading(Vectorf AccelMeas)
{
    float ax, ay;
    // float az;  // Normalized
    float axsq, aysq, sqrtTerm;
    float mx, my, mz;  // Normalized
    float xterm, yterm;
    float invMagn;

    // Normalize accelerometer measurements
    invMagn = 1.0f / AccelMeas.GetNorm();
    ax = AccelMeas.vec[0] * invMagn;
    ay = AccelMeas.vec[1] * invMagn;
    // az = AccelMeas.vec[2] * invMagn;

    // Normalize magnetometer measurements
    invMagn = 1.0f / Mag.GetNorm();
    mx = Mag.vec[0] * invMagn;
    my = Mag.vec[1] * invMagn;
    mz = Mag.vec[2] * invMagn;

    // Tilt-compensate magnetometer
    axsq = ax * ax;
    aysq = ay * ay;
    sqrtTerm = sqrtf(1.0f - (axsq) - (aysq));
    
    xterm = (mx * (1.0f - (axsq))) - (my * ax * ay) - (mz * ax * sqrtTerm);
    yterm = (my * sqrtTerm) - (mz * ay);
    heading = atan2f(yterm, xterm);

    // Perform checks
    if (heading <= FLOAT_PREC_ZERO)
        heading += CONSTS_2PI;

    if (heading >= CONSTS_2PI)  // Just in case
        heading -= CONSTS_2PI;
    
    heading = RangeConstrain(heading, 0.0f, CONSTS_2PI);

    return heading;


}



/* Deconstructor */
MagCompass::~MagCompass() {};

/* GetInstance() function for singleton. Do not use as user */
MagCompass &MagCompass::GetInstance()
{
    static MagCompass instance;
    return instance;
}

MagCompass &Compass = Compass.GetInstance();
