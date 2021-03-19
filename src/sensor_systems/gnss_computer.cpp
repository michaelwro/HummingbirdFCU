




#include "sensor_systems/gnss_computer.h"


GNSSComputer::GNSSComputer(HardwareSerial *userSerial)
{
    isConfigured = false;
    gpsSerial = userSerial;
}



bool GNSSComputer::ConfigureDevice(
    GNSSBaudRates_t userBaud, GNSSNetworks_t userNetwork,
    GNSSDynamics_t userDynModel, GNSSNavRate_t userODR)
{
    uint32_t pauseBetweenTasks = 100;  // [ms] millis() between configuration tasks
    
    isConfigured = false;

    // Permissible nav. rate + network combinations:
    // 5Hz + GPS + GLONASS
    // 5Hz/10Hz + GPS
    // 5Hz/10Hz + GLONASS
    if (userNetwork == GNSS_NET_GPS_GLONASS && userODR > GNSS_NAVRATE_5HZ)
    {
        #ifdef GNSS_DEBUG
        DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Cannot have >5Hz nav. rate and GPS+GLONASS for NEO-M8N, ");
        DEBUG_PORT.println("  defaulting to 5Hz and GPS+GLONASS.");
        #endif
        userNetwork = GNSS_NET_GPS_GLONASS;
        userODR = GNSS_NAVRATE_5HZ;
    }

    GPS_PORT.begin(GNSS_DEFAULT_BAUD);
    delay(250);  // Wait a sec

    GPS_PORT.flush();
    GPS_PORT.begin(GNSS_DEFAULT_BAUD);  // Reconnect
    delay(250);

    /* Config baud rate */
    switch (userBaud)
    {
        case GNSS_BAUD_9600: {
            SendUBXConfigMessage(UBX_CFG_PRT_9600B, UBX_CFG_PRTLEN);
            gpsBaud = 9600;
            break;
        }
        case GNSS_BAUD_38400: {
            SendUBXConfigMessage(UBX_CFG_PRT_38400B, UBX_CFG_PRTLEN);
            gpsBaud = 38400;
            break;
        }
        case GNSS_BAUD_115200: {
            SendUBXConfigMessage(UBX_CFG_PRT_115200B, UBX_CFG_PRTLEN);
            gpsBaud = 115200;
            break;
        }
        case GNSS_BAUD_230400: {
            SendUBXConfigMessage(UBX_CFG_PRT_230400B, UBX_CFG_PRTLEN);
            gpsBaud = 230400;
            break;
        }
        default: {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Unknown baud rate, defaulting to 115200");
            #endif
            SendUBXConfigMessage(UBX_CFG_PRT_115200B, UBX_CFG_PRTLEN);
            gpsBaud = 115200;
            break;
        }
    }

    delay(pauseBetweenTasks);  // Wait a sec
    GPS_PORT.flush();
    GPS_PORT.begin(gpsBaud);  // Reconnect
    delay(500);

    #ifdef GNSS_DEBUG
    DEBUG_PORT.print("GNSSComputer::ConfigureDevice: Changed baud to ");
    DEBUG_PORT.print(gpsBaud);
    DEBUG_PORT.println(" and reconnected.");
    #endif

    // if (!GPS_PORT)
    // {
    //     #ifdef GNSS_DEBUG
    //     DEBUG_PORT.println("GNSSComputer::ConfigureDevice ERROR: Could not reconnect to GPS after changing baud.");
    //     #endif
    //     return false;
    // }


    /* Config. satellite network */
    switch (userNetwork)
    {
        case GNSS_NET_GPS: {
            SendUBXConfigMessage(UBX_CFG_GNSS_GPS, UBX_CFG_GNSSLEN);
            network = GNSS_NET_GPS;
            break;
        }
        case GNSS_NET_GLONASS: {
            SendUBXConfigMessage(UBX_CFG_GNSS_GLONASS, UBX_CFG_GNSSLEN);
            network = GNSS_NET_GLONASS;
            break;
        }
        case GNSS_NET_GPS_GLONASS: {
            SendUBXConfigMessage(UBX_CFG_GNSS_GPS_GLONASS, UBX_CFG_GNSSLEN);
            network = GNSS_NET_GPS_GLONASS;
            break;
        }
        default: {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Unknown network, defaulting to GPS+GLONASS");
            #endif
            SendUBXConfigMessage(UBX_CFG_GNSS_GPS_GLONASS, UBX_CFG_GNSSLEN);
            network = GNSS_NET_GPS_GLONASS;
            break;
        }
    }

    delay(pauseBetweenTasks);

    #ifdef GNSS_DEBUG
    DEBUG_PORT.println("GNSSComputer::ConfigureDevice: Changed network.");
    #endif

    
    /* Config dynamic model */
    switch (userDynModel)
    {
        case GNSS_DYNAMICS_PORTABLE: {
            SendUBXConfigMessage(UBX_CFG_NAV5_PORTABLE_3D, UBX_CFG_NAV5LEN);
            dynamicModel = GNSS_DYNAMICS_PORTABLE;
            break;
        }
        case GNSS_DYNAMICS_PEDESTRIAN: {
            SendUBXConfigMessage(UBX_CFG_NAV5_PEDESTRIAN_3D, UBX_CFG_NAV5LEN);
            dynamicModel = GNSS_DYNAMICS_PEDESTRIAN;
            break;
        }
        case GNSS_DYNAMICS_AIRBORNE_1G: {
            SendUBXConfigMessage(UBX_CFG_NAV5_AIR1G_3D, UBX_CFG_NAV5LEN);
            dynamicModel = GNSS_DYNAMICS_AIRBORNE_1G;
            break;
        }
        default: {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Unknown dynamic model, defaulting to pedestrian");
            #endif
            SendUBXConfigMessage(UBX_CFG_NAV5_PEDESTRIAN_3D, UBX_CFG_NAV5LEN);
            dynamicModel = GNSS_DYNAMICS_PEDESTRIAN;
            break;
        }
    }

    delay(pauseBetweenTasks);

    #ifdef GNSS_DEBUG
    DEBUG_PORT.println("GNSSComputer::ConfigureDevice: Changed dynamic model.");
    #endif

    


    /* Config NMEA messages */
    SendUBXConfigMessage(UBX_CFG_MSG_DISABLE_GLL, 16);
    delay(pauseBetweenTasks);

    SendUBXConfigMessage(UBX_CFG_MSG_DISABLE_GSV, 16);
    delay(pauseBetweenTasks);

    SendUBXConfigMessage(UBX_CFG_MSG_DISABLE_RMC, 16);
    delay(pauseBetweenTasks);

    #ifdef GNSS_DEBUG
    DEBUG_PORT.println("GNSSComputer::ConfigureDevice: Disabled GxGLL, GxGSV, and GxRMC NMEA messages.");
    #endif


    /* Config navigation rate */
    switch (userODR)
    {
        case GNSS_NAVRATE_5HZ: {
            SendUBXConfigMessage(UBX_CFG_RATE_5HZ, UBX_CFG_RATELEN);
            updateRate = GNSS_NAVRATE_5HZ;
            navRate = 5.0f;
            break;
        }
        case GNSS_NAVRATE_10HZ: {
            SendUBXConfigMessage(UBX_CFG_RATE_10HZ, UBX_CFG_RATELEN);
            updateRate = GNSS_NAVRATE_10HZ;
            navRate = 10.0f;
            break;
        }
        default: {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Unknown nav rate, defaulting to 5Hz");
            #endif
            SendUBXConfigMessage(UBX_CFG_RATE_5HZ, UBX_CFG_RATELEN);
            updateRate = GNSS_NAVRATE_5HZ;
            navRate = 5.0f;
            break;
        }
    }

    navTs = 1.0f / navRate;
    delay(pauseBetweenTasks);
    #ifdef GNSS_DEBUG
    DEBUG_PORT.print("GNSSComputer::ConfigureDevice: Changed nav rate to ");
    DEBUG_PORT.print(navRate);
    DEBUG_PORT.println("Hz");
    #endif

    isConfigured = true;
    return true;
}



bool GNSSComputer::WaitForSatellites(uint32_t minSats)
{
    // bool completeMessage;
    // char b;
    // int i;
    // uint32_t nSats;
    // uint32_t startMillis;
    // uint32_t currMillis;
    // uint32_t waitTimeout = 120000;  // 2min. [ms] How long to wait for nSat connections
    // int nbytes;

    // #ifdef GNSS_DEBUG
    // uint32_t prevPrint, currPrint;
    // prevPrint = 0;
    // currPrint = millis();
    // DEBUG_PORT.println("GNSSComputer::WaitForSatellites: Waiting for satellites.");
    // DEBUG_PORT.print("  Sats: ");
    // #endif

    // startMillis = millis();
    // currMillis = millis();
    // nSats = 0;
    // completeMessage = false;
    while (1)
    {
        // currMillis = millis();

        while (GPS_PORT.available())
        {
            char b = GPS_PORT.read();
            // completeMessage = gpsDevice.encode(b);
            // gpsDevice.encode(b);
            DEBUG_PORT.print(b);

            if (b == '\n')
                DEBUG_PORT.println();

            // New message with updated nsats

            // if (completeMessage)
            // {
            //     #ifdef GNSS_DEBUG
            //     DEBUG_PORT.print(".");
            //     #endif
            // }
        }
    
        // if (gpsDevice.satellites.isUpdated() && gpsDevice.satellites.isValid())
        // {
        //     nSats = gpsDevice.satellites.value();

        //     #ifdef GNSS_DEBUG
        //     DEBUG_PORT.print("CHECK ");
        //     #endif
            
        //     if (nSats >= minSats)
        //     {
        //         #ifdef GNSS_DEBUG
        //         DEBUG_PORT.println(" Satellites acquired!");
        //         #endif
        //         return true;
        //     }
        // }

        // completeMessage = false;

        // #ifdef GNSS_DEBUG
        // currPrint = millis();
        // if (currPrint - prevPrint >= 1000)
        // {
        //     DEBUG_PORT.print(nSats);
        //     DEBUG_PORT.print(",");
        //     prevPrint = currPrint;
        // }
        // #endif

    }

    #ifdef GNSS_DEBUG
    DEBUG_PORT.println();
    DEBUG_PORT.print("GNSSComputer::WaitForSatellites ERROR: Took too long to find ");
    DEBUG_PORT.print(minSats);
    DEBUG_PORT.println(" sats.");
    #endif

    return false;
}


void GNSSComputer::SendUBXConfigMessage(const uint8_t *msg, size_t len)
{
    // Send over userSerial port
    GPS_PORT.write(msg, len);
    
    // Wait a bit after sending config command to the device
    delay(50);
}


/* GetInstance() function for singleton */
GNSSComputer &GNSSComputer::GetInstance()
{
    static GNSSComputer instance;
    return instance;
}

GNSSComputer &GPS = GPS.GetInstance();
