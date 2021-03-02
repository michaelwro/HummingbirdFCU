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


#include "state_estimation/inertial_nav_system.h"



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
AccelMagSensor(&SensorI2C), GyroSensor(&SensorI2C), 
AxMFilt(INS_ACCEL_MFILT_LEN), AyMFilt(INS_ACCEL_MFILT_LEN),
AzMFilt(INS_ACCEL_MFILT_LEN)
{
    fs = 200.0f;  // Default, just in case
    ts = 1.0f / fs;
    tsMillis = (uint32_t)(ts * 1000.0f);
    prevUpdateMicros = micros();

}


// ----------------------------------------------------------------------------
// SetSampleRate(float fs_hz)
// ----------------------------------------------------------------------------
/**
 * Set the INS sample rate.
 * 
 * @param fs_hz Sample rate in [Hz]
 */
void InertialNavSystem::SetSampleRate(float fs_hz)
{
    fs = fs_hz;
    ts = 1.0f / fs_hz;
    tsMillis = (uint32_t)(ts * 1000.0f);
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
    // connect/config gyro
    // connect/config accel
    // get init gyro biases
    // get init accel biases
    size_t n;

    #ifdef INS_DEBUG
    DebugPort.print("INERTIALNAVSYSTEM:Initialize: Connecting to sensors... ");
    #endif


    /* Connect to and initialize the gyro */
    if (!GyroSensor.Initialize(INS_GYRO_RANGE))
    {
        #ifdef INS_DEBUG
            DebugPort.println("INERTIALNAVSYSTEM:Initialize ERROR: Could not initialize/connect to FXAS21002 gyro. Check settings.");
        #endif
        return false;
    }


    /* Connect to and initialize the accelerometer */
    if (!AccelMagSensor.Initialize(INS_ACCEL_RANGE))
    {
        #ifdef INS_DEBUG
            DebugPort.println("INERTIALNAVSYSTEM:Initialize ERROR: Could not initialize/connect to FXOS8700 accelerometer. Check settings.");
        #endif
        return false;
    }

    #ifdef INS_DEBUG
    DebugPort.println("Done!");
    #endif


    /* Compute initial gyro turn-on biases */
    #ifdef INS_DEBUG
    DebugPort.print("INERTIALNAVSYSTEM:Initialize: Measuring gyro turn-on biases... ");
    #endif

    n = (size_t)(fs * INS_BIAS_INIT_TIME);  // Number of readings
    if (!ComputeInitGyroBiases(n))
    {
        #ifdef INS_DEBUG
        DebugPort.println("INERTIALNAVSYSTEM:Initialize ERROR: Error computing gyro turn-on biases.");
        #endif
        return false;
    }

    #ifdef INS_DEBUG
    DebugPort.println("Done!");
    DebugPort.print("    BGX: "); DebugPort.println(GyroTOBias.vec[0], 4);
    DebugPort.print("    BGY: "); DebugPort.println(GyroTOBias.vec[1], 4);
    DebugPort.print("    BGZ: "); DebugPort.println(GyroTOBias.vec[2], 4);
    #endif


    /* Compute initial accelerometer turn-on biases */
    #ifdef INS_DEBUG
    DebugPort.print("INERTIALNAVSYSTEM:Initialize: Measuring accelerometer turn-on biases... ");
    #endif

    if (!ComputeInitAccelBiases(n))
    {
        #ifdef INS_DEBUG
        DebugPort.println("INERTIALNAVSYSTEM:Initialize ERROR: Error computing accelerometer turn-on biases.");
        #endif
        return false;
    }

    #ifdef INS_DEBUG
    DebugPort.println("Done!");
    DebugPort.print("    BAX: "); DebugPort.println(AccelTOBias.vec[0], 4);
    DebugPort.print("    BAY: "); DebugPort.println(AccelTOBias.vec[1], 4);
    DebugPort.print("    BAZ: "); DebugPort.println(AccelTOBias.vec[2], 4);
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
    float ax, ay, az;
    float g;
    
    /* Read gyro sensor */
    if (!GyroSensor.ReadSensor())
    {
        #ifdef INS_DEBUG
        DebugPort.println("INERTIALNAVSYSTEM:Update ERROR: Could not read gyro sensor.");
        #endif
        // return false;
    }

    gx = GyroSensor.GetGx();
    gy = GyroSensor.GetGy();
    gz = GyroSensor.GetGz();
    GyroRaw.vec[0] = gx;
    GyroRaw.vec[1] = gy;
    GyroRaw.vec[2] = gz;


    /* Read accelerometer sensor */
    if (!AccelMagSensor.ReadSensor())
    {
        #ifdef INS_DEBUG
        DebugPort.println("INERTIALNAVSYSTEM:Update ERROR: Could not read accelerometer sensor.");
        #endif
        // return false;
    }

    g = GravComputer.GetGravity();
    axRaw = AccelMagSensor.GetAx() * g;
    ayRaw = AccelMagSensor.GetAy() * g;
    azRaw = AccelMagSensor.GetAz() * g;
    AccelRaw.vec[0] = axRaw;
    AccelRaw.vec[1] = ayRaw;
    AccelRaw.vec[2] = azRaw;

    prevUpdateMicros = micros();

    /* Apply calibration */
    ax = axRaw - SENSCALIB_ACCEL_BX;
    ay = ayRaw - SENSCALIB_ACCEL_BY;
    az = azRaw - SENSCALIB_ACCEL_BZ;


    /* Apply filters */
    Gyro.vec[0] = gx;
    Gyro.vec[1] = gy;
    Gyro.vec[2] = gz;
    Accel.vec[0] = AxMFilt.Filter(ax);
    Accel.vec[1] = AyMFilt.Filter(ay);
    Accel.vec[2] = AzMFilt.Filter(az);


    /* Update accelerometer tilt angles */
    UpdateAccelAngles();

    return true;
}



/* Return the accelerometer pitch (phi) angle in [rad] */
float InertialNavSystem::GetAccelPitch()
{
    return pitch;
}

/* Return the accelerometer roll (theta) angle in [rad] */
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
    pitch = asinf_safe(ax / sqrtf((ax * ax) + (ay * ay) + (az * az)));
    // pitch = asinf_safe(ax / GravComputer.GetGravity());
    roll = atan2f(ay, az);
}


// ----------------------------------------------------------------------------
// ComputeInitGyroBiases(size_t n)
// ----------------------------------------------------------------------------
/**
 * Measure initial gyro turn-on bias.
 * 
 * @param n Number of measurements to take
 * @returns True if successful, false if not
 */
bool InertialNavSystem::ComputeInitGyroBiases(size_t n)
{
    size_t i;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    for (i = 0; i < n; i++)
    {
        if (!Update())
        {
            #ifdef INS_DEBUG
            DebugPort.println("INERTIALNAVSYSTEM:ComputeInitGyroBiases ERROR: Could not read sensors.");
            #endif
            return false;
        }
        x += Gyro.vec[0];
        y += Gyro.vec[1];
        z += Gyro.vec[2];
        delay(tsMillis);
    }

    GyroTOBias.vec[0] = x / (float)n;
    GyroTOBias.vec[1] = y / (float)n;
    GyroTOBias.vec[2] = z / (float)n;

    return true;
}


// ----------------------------------------------------------------------------
// ComputeInitAccelBiases(size_t n)
// ----------------------------------------------------------------------------
/**
 * Measure initial accelerometer turn-on bias.
 * 
 * @param n Number of measurements to take
 * @returns True if successful, false if not
 */
bool InertialNavSystem::ComputeInitAccelBiases(size_t n)
{
    size_t i;
    float g;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float sroll, croll;
    float spitch, cpitch;
    float axNED, ayNED, azNED;


    // Compute average accel values
    for (i = 0; i < n; i++)
    {
        if (!Update())
        {
            #ifdef INS_DEBUG
            DebugPort.println("INERTIALNAVSYSTEM:ComputeInitAccelBiases ERROR: Could not read sensors.");
            #endif
            return false;
        }
        x += Accel.vec[0];
        y += Accel.vec[1];
        z += Accel.vec[2];
        delay(tsMillis);
    }

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
