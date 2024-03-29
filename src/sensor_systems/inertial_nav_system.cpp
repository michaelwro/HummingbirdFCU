// ----------------------------------------------------------------------------
// ACCELEROMETER, GYRO, AND MAGNETOMETER INERTIAL SENSOR SYSTEM
// 
// Code By: Michael Wrona
// Created: 27 Feb 2021
// ----------------------------------------------------------------------------
/**
 * The inertial sensor software system brings together the accelerometer  
 * and gyro, sensors to form the Inertial Navigation System. This system 
 * provides an interface to manage sensor readings, filtering raw measurements 
 * for noise, and applying calibration parameters.
 */


#include "sensor_systems/inertial_nav_system.h"



// ----------------------------------------------------------------------------
// InertialNavSystem()
// ----------------------------------------------------------------------------
/**
 * Constructor for the InertialNavSystem class. This system provides an 
 * interface to manage sensor readings, filtering raw measurements for noise, 
 * and applying calibration parameters.
 */
InertialNavSystem::InertialNavSystem()
: Gyro(3), GyroRaw(3), GyroTOBias(3),
Accel(3), AccelRaw(3), AccelTOBias(3), 
AccelMagSensor(&SENSOR_I2C), GyroSensor(&SENSOR_I2C)
{
    prevUpdateMicros = micros();
}


// ----------------------------------------------------------------------------
// Initialize()
// ----------------------------------------------------------------------------
/**
 * Initialize the INS. This function connects to the gyro and accelerometer, 
 * and measures gyro and accelerometer turn-on biases (used to initialize the 
 * EKF).
 * 
 * @returns true if successfully init'd, false if not.
 */
bool InertialNavSystem::Initialize()
{
    uint32_t initBiasTS = 20;  // Sample period for measuring init. IMU biases.

    #ifdef INS_DEBUG
    DEBUG_PRINTLN("INERTIALNAVSYSTEM::Initialize: Connecting to sensors.");
    #endif

    /* Connect to and initialize the gyro */
    if (!GyroSensor.Initialize(INS_GYRO_RANGE))
    {
        #ifdef INS_DEBUG
        DEBUG_PRINTLN("INERTIALNAVSYSTEM::Initialize ERROR: Could not initialize/connect to FXAS21002 gyro. Check settings.");
        #endif
        return false;
    }

    /* Connect to and initialize the accelerometer */
    if (!AccelMagSensor.Initialize(INS_ACCEL_RANGE))
    {
        #ifdef INS_DEBUG
        DEBUG_PRINTLN("INERTIALNAVSYSTEM:Initialize ERROR: Could not initialize/connect to FXOS8700 accelerometer. Check settings.");
        #endif
        return false;
    }


    /* Init accelerometer filters */
    AxLPF.SetSmoothingFactor(INS_ACCEL_LPF_SF);
    AyLPF.SetSmoothingFactor(INS_ACCEL_LPF_SF);
    AzLPF.SetSmoothingFactor(INS_ACCEL_LPF_SF);


    /* Compute initial gyro turn-on biases */
    #ifdef INS_DEBUG
    DEBUG_PRINTLN("INERTIALNAVSYSTEM::Initialize: Measuring gyro turn-on biases (rad/s)...");
    #endif

    if (!MeasureInitGyroBiases(initBiasTS))
    {
        #ifdef INS_DEBUG
        DEBUG_PRINTLN("INERTIALNAVSYSTEM::Initialize ERROR: Error computing gyro turn-on biases...");
        #endif
        return false;
    }

    #ifdef INS_DEBUG
    DEBUG_PRINT("    BGX0: "); DEBUG_PRINTLNF(GyroTOBias.vec[0], 4);
    DEBUG_PRINT("    BGY0: "); DEBUG_PRINTLNF(GyroTOBias.vec[1], 4);
    DEBUG_PRINT("    BGZ0: "); DEBUG_PRINTLNF(GyroTOBias.vec[2], 4);
    #endif


    /* Compute initial accelerometer turn-on biases */
    #ifdef INS_DEBUG
    DEBUG_PRINTLN("INERTIALNAVSYSTEM::Initialize: Measuring accelerometer turn-on biases (g)...");
    #endif

    if (!MeasureInitAccelBiases(initBiasTS))
    {
        #ifdef INS_DEBUG
        DEBUG_PRINTLN("INERTIALNAVSYSTEM::Initialize ERROR: Error computing accelerometer turn-on biases.");
        #endif
        return false;
    }

    #ifdef INS_DEBUG
    DEBUG_PRINT("    BAX0: "); DEBUG_PRINTLNF(AccelTOBias.vec[0], 4);
    DEBUG_PRINT("    BAY0: "); DEBUG_PRINTLNF(AccelTOBias.vec[1], 4);
    DEBUG_PRINT("    BAZ0: "); DEBUG_PRINTLNF(AccelTOBias.vec[2], 4);
    #endif

    return true;

}


// ----------------------------------------------------------------------------
// Update()
// ----------------------------------------------------------------------------
/**
 * Record accelerometer and gyro measurements, apply noise filters, and update 
 * accel. roll/pitch angles.
 * 
 * @returns True if success, false if not.
 */
bool InertialNavSystem::Update()
{
    float gx, gy, gz;
    float axRaw, ayRaw, azRaw;
    float bax, bay, baz;
    float g;
    
    /* Read gyro sensor */
    if (!GyroSensor.ReadSensor())
    {
        #ifdef INS_DEBUG
        DEBUG_PRINTLN("INERTIALNAVSYSTEM::Update ERROR: Could not read gyro sensor.");
        #endif
        // return false;
    }
    
    gx = GyroSensor.GetGx();
    gy = GyroSensor.GetGy();
    gz = GyroSensor.GetGz();
    GyroRaw.vec[0] = gx;  // In [deg/s]
    GyroRaw.vec[1] = gy;
    GyroRaw.vec[2] = gz;

    // Convert to radians
    // TODO: apply filter?
    gx *= DEG2RAD;
    gy *= DEG2RAD;
    gz *= DEG2RAD;
    Gyro.vec[0] = gx;
    Gyro.vec[1] = gy;
    Gyro.vec[2] = gz;


    /* Read accelerometer sensor */
    if (!AccelMagSensor.ReadSensor())
    {
        #ifdef INS_DEBUG
        DEBUG_PRINTLN("INERTIALNAVSYSTEM::Update ERROR: Could not read accelerometer sensor.");
        #endif
        // return false;
    }

    /* Retrieve and log raw accel. values */
    axRaw = AccelMagSensor.GetAx();
    ayRaw = AccelMagSensor.GetAy();
    azRaw = AccelMagSensor.GetAz();
    AccelRaw.vec[0] = axRaw;  // In G's
    AccelRaw.vec[1] = ayRaw;
    AccelRaw.vec[2] = azRaw;

    /* Apply calibration (in g's)*/
    bax = axRaw - SENSCALIB_ACCEL_BX;
    bay = ayRaw - SENSCALIB_ACCEL_BY;
    baz = azRaw - SENSCALIB_ACCEL_BZ;
    Accel.vec[0] = (SENSCALIB_ACCEL_S11 * bax) + (SENSCALIB_ACCEL_S12 * bay) + (SENSCALIB_ACCEL_S13 * baz);
    Accel.vec[1] = (SENSCALIB_ACCEL_S12 * bax) + (SENSCALIB_ACCEL_S22 * bay) + (SENSCALIB_ACCEL_S23 * baz);
    Accel.vec[2] = (SENSCALIB_ACCEL_S13 * bax) + (SENSCALIB_ACCEL_S23 * bay) + (SENSCALIB_ACCEL_S33 * baz);

    /* Convert from g's to m/s/s */
    g = GravComputer.GetGravity();
    Accel.vec[0] *= g;
    Accel.vec[1] *= g;
    Accel.vec[2] *= g;

    /* Apply filter */
    Accel.vec[0] = AxLPF.Filter(Accel.vec[0]);
    Accel.vec[1] = AyLPF.Filter(Accel.vec[1]);
    Accel.vec[2] = AzLPF.Filter(Accel.vec[2]);

    prevUpdateMicros = micros();

    /* Update accelerometer tilt angles */
    UpdateAccelAngles();

    return true;
}



/* Return the accelerometer NED pitch (phi) angle in [rad] */
float InertialNavSystem::GetAccelPitch()
{
    return pitch;
}

/* Return the accelerometer NED roll (theta) angle in [rad] */
float InertialNavSystem::GetAccelRoll()
{
    return roll;
}


// ------------------------------------
// Private methods
// ------------------------------------


// ----------------------------------------------------------------------------
// UpdateAccelAngles()
// ----------------------------------------------------------------------------
/**
 * Compute accelerometer roll and pitch angles.
 */
void InertialNavSystem::UpdateAccelAngles()
{
    float ax = Accel.vec[0];
    float ay = Accel.vec[1];
    float az = Accel.vec[2];
    float magn = Accel.GetNorm();
    
    pitch = asinf_safe(ax / magn);
    // pitch = asinf_safe(ax / GravComputer.GetGravity());
    roll = atan2f(ay, az);
}


// ----------------------------------------------------------------------------
// InertialNavSystem::MeasureInitGyroBiases(uint32_t samplePeriod)
// ----------------------------------------------------------------------------
/**
 * Measure initial gyro turn-on biases.
 * 
 * @param samplePeriod  [ms] Sample period/delay between samples. Use this to 
 * set sample rate for recording turn-on biases.
 * @returns True if successful, false if not
 */
bool InertialNavSystem::MeasureInitGyroBiases(uint32_t samplePeriod)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    uint32_t n = 0;  // number of measurements
    uint32_t prev;  // previous measurement millis()
    uint32_t start;  // starting millis()

    uint32_t now = millis();
    prev = now;
    start = now;
    while (now - start <= INS_BIAS_INIT_TIME)
    {
        now = millis();
        if (now - prev >= samplePeriod)  // Take a measurement
        {
            if (!Update())  // Take IMU measurements
            {
                #ifdef INS_DEBUG
                DEBUG_PRINTLN("INERTIALNAVSYSTEM::ComputeInitGyroBiases ERROR: Could not read sensors.");
                #endif
                return false;
            }
            x += Gyro.vec[0];  // Increase average counter
            y += Gyro.vec[1];  // [rad]
            z += Gyro.vec[2];
            n++;  // number of measurements

            prev = now;  // update measurement timers
        }
    }

    // Compute averages
    GyroTOBias.vec[0] = x / (float)n;
    GyroTOBias.vec[1] = y / (float)n;
    GyroTOBias.vec[2] = z / (float)n;

    return true;
}


// ----------------------------------------------------------------------------
// InertialNavSystem::MeasureInitAccelBiases(uint32_t samplePeriod)
// ----------------------------------------------------------------------------
/**
 * Measure initial accelerometer turn-on bias.
 * 
 * @param samplePeriod  [ms] Sample period/delay between samples. Use this to 
 * set sample rate for recording turn-on biases.
 * @returns True if successful, false if not
 */
bool InertialNavSystem::MeasureInitAccelBiases(uint32_t samplePeriod)
{
    float g;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float sroll, croll;
    float spitch, cpitch;
    float axNED, ayNED, azNED;
    uint32_t n = 0;  // number of measurements
    uint32_t prev;  // previous measurement millis()
    uint32_t start;  // starting millis()

    uint32_t now = millis();
    prev = now;
    start = now;
    while (now - start <= INS_BIAS_INIT_TIME)
    {
        now = millis();
        if (now - prev >= samplePeriod)  // Take a measurement
        {
            if (!Update())  // Take IMU measurements
            {
                #ifdef INS_DEBUG
                DEBUG_PRINTLN("INERTIALNAVSYSTEM::ComputeInitGyroBiases ERROR: Could not read sensors.");
                #endif
                return false;
            }
            x += Accel.vec[0];  // Increase average counter
            y += Accel.vec[1];  // [m/s/s]
            z += Accel.vec[2];
            n++;  // number of measurements

            prev = now;  // update measurement timers
        }
    }

    // Compute averages
    x /= (float)n;
    y /= (float)n;
    z /= (float)n;

    // Compute gravity vector in body frame from accel measurements
    // Gravity in the NED frame is [0, 0, g]
    g = GravComputer.GetGravity();
    pitch = asinf_safe(x / sqrtf((x * x) + (y * y) + (z * z)));
    roll = atan2f(y, z);

    sroll = sinf(roll);
    croll = cosf(roll);
    spitch = sinf(pitch);
    cpitch = cosf(pitch);

    // Ref: https://youtu.be/p7tjtLkIlFo?t=244
    axNED = -g * spitch;
    ayNED = g * cpitch * sroll;
    azNED = g * cpitch * croll;

    // true = meas - bias  =>  bias = meas - true
    AccelTOBias.vec[0] = x - axNED;
    AccelTOBias.vec[1] = y - ayNED;
    AccelTOBias.vec[2] = z - azNED;

    return true;
}



/* Deconstructor */
InertialNavSystem::~InertialNavSystem() {}

/* GetInstance() function for IMS singleton. Don't use as a user. */
InertialNavSystem &InertialNavSystem::GetInstance()
{
    static InertialNavSystem instance;
    return instance;
}

InertialNavSystem &INS = INS.GetInstance();
