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
#include "sensor_drivers/fxos8700_accelmag.h"  // [] COMPLETED
#include "sensor_drivers/fxas21002_gyro.h"  // [] COMPLETED?
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
#include "sensor_systems/inertial_nav_system.h"
#include "sensor_systems/compass.h"



#include "TinyGPS++.h"
#include "sensor_systems/gnss_computer.h"

// #include "gravity_computer.h"











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
unsigned long gpstimer1 = 0;
unsigned long gpstimer2 = 0;


void setup()
{
    // Initialize serial ports
    // GPSSerial.begin(9600);

    DEBUG_PORT.begin(115200);
    delay(1000);

    #ifdef DEBUG
        while (!DEBUG_PORT);  // Wait for console to open
    #endif

    
    /* SET UP LEDS */
    pinMode(RED_LED, OUTPUT);
    pinMode(GRN_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);  // Start off LOW
    digitalWrite(GRN_LED, LOW);  // digitalWrite(GRN_LED, LOW);

    // if (!INS.Initialize())
    // {
    //     DEBUG_PORT.println("Could not init. INS...");
    // }


    // if (!Compass.Initialize())
    // {
    //     DEBUG_PORT.println("Could not init. compass...");
    // }

    // if (!GPS.ConfigureDevice())
    // {
    //     DEBUG_PORT.println("ERROR CONFIGGING!");
    // }



    // // while (!GPS_PORT) {;}

    // if (!GPS.WaitForSatellites())
    // {
    //     DEBUG_PORT.println("ERROR WAITING FOR SATS!");
    // }



    // GPS_PORT.begin(9600);
    // delay(1000);

    // while (!GPS_PORT) {;}

    // TinyGPSPlus gps;
    // while (1)
    // {
    //     while (GPS_PORT.available()) {
    //         char b =  GPS_PORT.read();
    //         DEBUG_PORT.print(b);
    //         // if (b == '\n')
    //         //     DEBUG_PORT.println();
    //     }
    // }

        
    //     if (gps.satellites.isUpdated())
    //     {
    //         DEBUG_PORT.println("-------------------------------------------");
    //         DEBUG_PORT.print(F("SATELLITES Fix Age="));
    //         DEBUG_PORT.print(gps.satellites.age());
    //         DEBUG_PORT.print(F("ms Value="));
    //         DEBUG_PORT.println(gps.satellites.value());
    //         DEBUG_PORT.println("-------------------------------------------");
    //     }
    // }





    #ifdef DEBUG
        DEBUG_PORT.println("DONE!");
    #endif

    digitalWrite(RED_LED, LOW);
    digitalWrite(GRN_LED, HIGH);


    // gpstimer1 = millis();
    GPS_PORT.begin(115200);
    SENSOR_I2C.begin();
}


void loop()
{
    // https://forum.arduino.cc/index.php?topic=713603.0
    uint8_t b;
    do
    {
        SENSOR_I2C.requestFrom(0x42, 1);
        b = SENSOR_I2C.read();

        if (b != 0xFF)
        {
            DEBUG_PORT.write(b);
            DEBUG_PORT.print(" ");
        }
    }
    while (true);
    
    
    // while (GPS_PORT.available())
    // {
    //     uint8_t b = GPS_PORT.read();
    //     gpstimer1++;
    //     DEBUG_PORT.print("Count: ");
    //     DEBUG_PORT.println(gpstimer1);
    // }

    // gpstimer1 = 0;
    // delay(100);

    // now = millis();
    // if (now - prev >= 20)
    // {
    //     if (GPS.ListenForData())
    //     {
    //         gpstimer2 = millis();
    //         DEBUG_PORT.print("Time between: ");
    //         DEBUG_PORT.println(gpstimer2 - gpstimer1);
    //         gpstimer1 = gpstimer2;
    //     }

    //     prev = now;
    // }

    // now = millis();
    // if (now - prev > 100)
    // {
    //     INS.Update();
    //     Compass.Update();
    //     float h = Compass.GetHeading(INS.Accel);
    //     DEBUG_PORT.print("Heading: "); DEBUG_PORT.print(h*RAD2DEG, 2); DEBUG_PORT.println(" deg.");
    //     prev = now;
    // }

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