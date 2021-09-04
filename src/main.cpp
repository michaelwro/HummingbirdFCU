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
#include "hummingbird_config.h"
#include "debugging.h"
#include "sensor_drivers/fxos8700_accelmag.h"
#include "sensor_drivers/fxas21002_gyro.h"
// #include "sensor_drivers/bmp388_barometer.h"
#include "maths/math_functs.h"
#include "maths/matrices.h"
#include "maths/vectors.h"
#include "filters/median_filter.h"
#include "filters/low_pass_filter.h"
#include "sensor_systems/inertial_nav_system.h"



// #include "TinyGPS++.h"
// #include "sensor_systems/gnss_computer.h"

// #include "gravity_computer.h"






unsigned long prev = 0;
unsigned long now = 0;



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

    // if (!baro.Initialize(BMP3_NO_OVERSAMPLING, BMP3_NO_OVERSAMPLING, BMP3_IIR_FILTER_COEFF_3, BMP3_ODR_100_HZ))
    // {
    //     DEBUG_PORT.println("ERROR INIT. SENSOR!");
    //     return;
    // }


    if (!INS.Initialize())
    {
        DEBUG_PORT.println("Could not init. INS...");
    }


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
        DEBUG_PORT.println("SETUP LOOP COMPLETE!");
    #endif

    digitalWrite(RED_LED, LOW);
    digitalWrite(GRN_LED, HIGH);
}


void loop()
{

    now = millis();
    if (now - prev >= 20)
    {
        if (!INS.Update())
        {
            DEBUG_PORT.println("ERROR READING INS!");
            return;
        }

        // float x = mag.GetMx();
        // float y = mag.GetMy();
        // float z = mag.GetMz();
        // DEBUG_PORT.print(x, 2); DEBUG_PORT.print(",");
        // DEBUG_PORT.print(y, 2); DEBUG_PORT.print(",");
        // DEBUG_PORT.println(z, 2);

        float ax = INS.Accel.vec[0];
        float ay = INS.Accel.vec[1];
        float az = INS.Accel.vec[2];
        float gx = INS.Gyro.vec[0];
        float gy = INS.Gyro.vec[1];
        float gz = INS.Gyro.vec[2];
        float roll = INS.GetAccelRoll() * RAD2DEG;
        float pitch = INS.GetAccelPitch() * RAD2DEG;

        // DEBUG_PRINT("Accel: ");
        // DEBUG_PRINTF(ax, 4);
        // DEBUG_PRINT(",");
        // DEBUG_PRINTF(ay, 4);
        // DEBUG_PRINT(",");
        // DEBUG_PRINTF(az, 4);
        // DEBUG_PRINT("   Gyro: ");
        // DEBUG_PRINTF(gx*RAD2DEG, 3);
        // DEBUG_PRINT(",");
        // DEBUG_PRINTF(gy*RAD2DEG, 3);
        // DEBUG_PRINT(",");
        // DEBUG_PRINTLNF(gz*RAD2DEG, 3);

        DEBUG_PRINT("Roll: ");
        DEBUG_PRINTF(roll, 3);
        DEBUG_PRINT("  Pitch: ");
        DEBUG_PRINTLNF(pitch, 3);
        prev = now;
    }



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

}

#endif
