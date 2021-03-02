// ============================================================================
// HUMMINGBIRD FCU QUADCOPTER FLIGHT CONTROLLER SOFTWARE
// VER. 1
// 
// Code By: Michael Wrona, B.S. Aerospace Engineering
// ============================================================================
/**
 * 
 * DEV'S TO-DO ITEMS
 * [] Range checks on accelerometer and magnetometer measurements
 * [] Error handling for sensor initializations
 * [] Range checks on gyro measurements
 * [] Range checks on barometer measurements
 * [x] Finalize median filter class
 * [] Finalize low-pass filter class
 * [] Investigate computing vertical speed with baro. sensor
 * [] Implement "Altimeter()" object for computing altitude, temperature, and vertical speed
 * 
 * 
 */





#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include "hummingbird_config.h"  // Custom config settings
#include "sensors/fxos8700_accelmag.h"  // [] COMPLETED
#include "sensors/fxas21002_gyro.h"  // [] COMPLETED?
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP3XX.h"
// #include "baro_altimeter.h"
#include "maths/math_functs.h"  // [] COMPLETED TODO: Check for ISNAN
#include "maths/matrix_math.h"
#include "maths/matrices.h"
#include "maths/vectors.h"
// #include "Conversions.h"
#include "filters/median_filter.h"  // [] COMPLETED
#include "filters/low_pass_filter.h"  // [] COMPLETED
#include "state_estimation/inertial_nav_system.h"



// #include "TinyGPS++.h"

#include "gravity_computer.h"













/* CREATE GLOBAL SENSOR OBJECTS */
// BaroAltimeter Altimeter;
// Adafruit_BMP3XX BaroTemp;
// FXOS8700AccelMag AccelMag(&Wire2);
// FXAS21002Gyro Gyro(&Wire2);
// Conversions Convert;

// #define GPSSerial Serial7
// TinyGPSPlus gps;
// static void smartDelay(unsigned long ms);


unsigned long prev = 0;
unsigned long now = 0;





void setup()
{
    // Initialize serial ports
    // GPSSerial.begin(9600);

    DebugPort.begin(115200);
    delay(1000);

    #ifdef DEBUG
        while (!DebugPort);  // Wait for console to open
    #endif

    
    /* SET UP LEDS */
    pinMode(RED_LED, OUTPUT);
    pinMode(GRN_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);  // Start off LOW
    digitalWrite(GRN_LED, LOW);  // digitalWrite(GRN_LED, LOW);

    if (!INS.Initialize())
    {
        DebugPort.println("Could not init. INS...");
    }


    #ifdef DEBUG
        DebugPort.println("DONE!");
    #endif

    digitalWrite(RED_LED, LOW);
    digitalWrite(GRN_LED, HIGH);
}


void loop()
{
    now = millis();
    if (now - prev > 250)
    {
        float bx, by, bz;
        float gx, gy, gz;
        INS.Update();

        gx = INS.Gyro.vec[0];
        gy = INS.Gyro.vec[1];
        gz = INS.Gyro.vec[2];

        DebugPort.print("GX: "); DebugPort.print(gx, 4); DebugPort.print("  ");
        DebugPort.print("GY: "); DebugPort.print(gy, 4); DebugPort.print("  ");
        DebugPort.print("GZ: "); DebugPort.print(gz, 4); DebugPort.print("  ");
        DebugPort.print("Norm: "); DebugPort.println(INS.Gyro.GetNorm(), 4);

        bx = INS.GyroTOBias.vec[0];
        by = INS.GyroTOBias.vec[1];
        bz = INS.GyroTOBias.vec[2];

        DebugPort.print("GX`: "); DebugPort.print(gx - bx, 4); DebugPort.print("  ");
        DebugPort.print("GY`: "); DebugPort.print(gy - by, 4); DebugPort.print("  ");
        DebugPort.print("GZ`: "); DebugPort.print(gz - bz, 4); DebugPort.print("  ");
        DebugPort.print("Norm`: "); DebugPort.println(sqrtf(powf(gx - bx, 2.0f) + powf(gy - by, 2.0f) + powf(gz - bz, 2.0f)), 4);
        DebugPort.println("--------------------------------------");

        // DebugPort.print("Roll: "); DebugPort.print(INS.GetAccelRoll() * RAD2DEG, 3); DebugPort.print("  ");
        // DebugPort.print("Pitch: "); DebugPort.println(INS.GetAccelPitch() * RAD2DEG, 3);
        prev = now;
    }

    // smartDelay(1000);
}

// // This custom version of delay() ensures that the gps object
// // is being "fed".
// static void smartDelay(unsigned long ms)
// {
//   unsigned long start = millis();
//   do 
//   {
//     while (GPSSerial.available())
//       gps.encode(GPSSerial.read());
//   } while (millis() - start < ms);
// }