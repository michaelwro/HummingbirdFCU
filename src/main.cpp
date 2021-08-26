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




#ifndef UNIT_TEST  // Use this main function if not unit testing
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



FXAS21002Gyro gyro;







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


uint8_t b;

TinyGPSPlus gps;


void setup()
{
    // Initialize serial ports
    // GPSSerial.begin(9600);

    DEBUG_PORT.begin(115200);
    GPS_I2C.setClock(400000);
    
    delay(1000);

    #ifdef DEBUG
        while (!DEBUG_PORT) {;}  // Wait for console to open
    #endif

    
    /* SET UP LEDS */
    pinMode(RED_LED, OUTPUT);
    pinMode(GRN_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);  // Start off LOW
    digitalWrite(GRN_LED, LOW);  // digitalWrite(GRN_LED, LOW);

    gyro.Initialize(GYRO_RNG_1000DPS);


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


    gpstimer1 = millis();
    // GPS_PORT.begin(115200);
    // GPS_I2C.begin();
}


void loop()
{

    now = millis();
    if (now - prev >= 200)
    {
        float temp = gyro.GetTemperature();
        DEBUG_PORT.print("Temp [C]: ");
        DEBUG_PORT.println(temp, 3);
        prev = now;
    }
    // // gpstimer2 = millis();
    // GPS.ListenForData();
    // // gpstimer1 = millis();

    // gpstimer2 = millis();

    
    
    // // (GPS.NMEAParser.location.isUpdated()
    // if (GPS.NMEAParser.time.isUpdated())
    // {
    //     // DEBUG_PORT.print("Lat: "); DEBUG_PORT.println(GPS.NMEAParser.location.rawLat().billionths);
    //     // DEBUG_PORT.print(" Lon: "); DEBUG_PORT.print(GPS.NMEAParser.location.rawLng().billionths);
    //     // DEBUG_PORT.print("Val: "); DEBUG_PORT.print(GPS.NMEAParser.time.value());
    //     // DEBUG_PORT.print(" TimeBetween: "); DEBUG_PORT.println(gpstimer2 - gpstimer1);
    //     // DEBUG_PORT.print(" TimeBetween: "); DEBUG_PORT.println(gpstimer1 - gpstimer2);
    //     gpstimer1 = gpstimer2;
    // }



    // // ====== Simple echo code ======
    // https://forum.arduino.cc/index.php?topic=713603.0
    // do {
    //     GPS_I2C.requestFrom(GNSS_I2C_ADDR, 1);
    //     b = GPS_I2C.read();
    //     if (b != 0xFF)
    //     {
    //         DEBUG_PORT.write(b);
    //         gps.encode((char)b);

    //         // if (gps.location.isUpdated())
    //         // {
    //         //     DEBUG_PORT.print("Sats: ");
    //         //     DEBUG_PORT.println(gps.location.lat(), 10);
    //         // }
    //     }
    // }
    // while (true);
    


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
#endif
