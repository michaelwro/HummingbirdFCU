




#include "sensor_systems/gnss_computer.h"


GNSSComputer::GNSSComputer(HardwareSerial *userSerial)
{
    isConfigured = false;
    gpsSerial = userSerial;
}



bool GNSSComputer::ConfigureDevice(GNSSBaudRates_t userBaud, GNSSNetworks_t userNetwork,
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

    // Check if serial connection is open or not
    if (!GPS_PORT)
    {
        #ifdef GNSS_DEBUG
        DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Serial port not open, connecting.");
        #endif
        GPS_PORT.begin(GNSS_DEFAULT_BAUD);
        delay(pauseBetweenTasks);
    }


    /* Config baud rate */
    switch (userBaud)
    {
        case GNSS_BAUD_9600: {
            SendUBXConfigMessage(UBX_CFG_PRT_9600B, 28);
            gpsBaud = 9600;
            break;
        }
        case GNSS_BAUD_38400: {
            SendUBXConfigMessage(UBX_CFG_PRT_38400B, 28);
            gpsBaud = 38400;
            break;
        }
        case GNSS_BAUD_115200: {
            SendUBXConfigMessage(UBX_CFG_PRT_115200B, 28);
            gpsBaud = 115200;
            break;
        }
        case GNSS_BAUD_230400: {
            SendUBXConfigMessage(UBX_CFG_PRT_230400B, 28);
            gpsBaud = 230400;
            break;
        }
        default: {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Unknown baud rate, defaulting to 115200");
            #endif
            SendUBXConfigMessage(UBX_CFG_PRT_115200B, 28);
            gpsBaud = 115200;
            break;
        }
    }

    GPS_PORT.end();  // Disconnect, then reconnect with new baud rate
    delay(100);  // Wait a sec
    GPS_PORT.begin(gpsBaud);  // Reconnect

    if (!GPS_PORT)
    {
        #ifdef GNSS_DEBUG
        DEBUG_PORT.println("GNSSComputer::ConfigureDevice ERROR: Could not reconnect to GPS after changing baud.");
        #endif
        return false;
    }

    GPS_PORT.flush();

    #ifdef GNSS_DEBUG
    DEBUG_PORT.print("GNSSComputer:ConfigureDevice: Changed baud to ");
    DEBUG_PORT.print(gpsBaud);
    DEBUG_PORT.println(" and reconnected.");
    #endif

    delay(pauseBetweenTasks);


    /* config. satellite network */
    switch (userNetwork)
    {
        case GNSS_NET_GPS: {
            SendUBXConfigMessage(UBX_CFG_GNSS_GPS, 68);
            network = GNSS_NET_GPS;
            break;
        }
        case GNSS_NET_GLONASS: {
            SendUBXConfigMessage(UBX_CFG_GNSS_GLONASS, 68);
            network = GNSS_NET_GLONASS;
            break;
        }
        case GNSS_NET_GPS_GLONASS: {
            SendUBXConfigMessage(UBX_CFG_GNSS_GPS_GLONASS, 68);
            network = GNSS_NET_GPS_GLONASS;
            break;
        }
        default: {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Unknown network, defaulting to GPS+GLONASS");
            #endif
            SendUBXConfigMessage(UBX_CFG_GNSS_GPS_GLONASS, 68);
            network = GNSS_NET_GPS_GLONASS;
            break;
        }
    }

    #ifdef GNSS_DEBUG
    DEBUG_PORT.println("GNSSComputer:ConfigureDevice: Changed network.");
    #endif

    delay(pauseBetweenTasks);


    /* Config dynamic model */
    switch (userDynModel)
    {
        case GNSS_DYNAMICS_PORTABLE: {
            SendUBXConfigMessage(UBX_CFG_NAV5_PORTABLE_3D, 44);
            dynamicModel = GNSS_DYNAMICS_PORTABLE;
            break;
        }
        case GNSS_DYNAMICS_PEDESTRIAN: {
            SendUBXConfigMessage(UBX_CFG_NAV5_PEDESTRIAN_3D, 44);
            dynamicModel = GNSS_DYNAMICS_PEDESTRIAN;
            break;
        }
        case GNSS_DYNAMICS_AIRBORNE_1G: {
            SendUBXConfigMessage(UBX_CFG_NAV5_AIR1G_3D, 44);
            dynamicModel = GNSS_DYNAMICS_AIRBORNE_1G;
            break;
        }
        default: {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Unknown dynamic model, defaulting to pedestrian");
            #endif
            SendUBXConfigMessage(UBX_CFG_NAV5_PEDESTRIAN_3D, 44);
            dynamicModel = GNSS_DYNAMICS_PEDESTRIAN;
            break;
        }
    }

    #ifdef GNSS_DEBUG
    DEBUG_PORT.println("GNSSComputer:ConfigureDevice: Changed dynamic model.");
    #endif

    delay(pauseBetweenTasks);


    /*Config navigation rate */
    switch (userODR)
    {
        case GNSS_NAVRATE_5HZ: {
            SendUBXConfigMessage(UBX_CFG_RATE_5HZ, 14);
            updateRate = GNSS_NAVRATE_5HZ;
            navRate = 5.0f;
            navTs = 1.0f / navRate;
            break;
        }
        case GNSS_NAVRATE_10HZ: {
            SendUBXConfigMessage(UBX_CFG_RATE_10HZ, 14);
            updateRate = GNSS_NAVRATE_5HZ;
            navRate = 10.0f;
            navTs = 1.0f / navRate;
            break;
        }
        default: {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ConfigureDevice WARNING: Unknown nav rate, defaulting to 5Hz");
            #endif
            SendUBXConfigMessage(UBX_CFG_RATE_5HZ, 14);
            updateRate = GNSS_NAVRATE_5HZ;
            navRate = 5.0f;
            navTs = 1.0f / navRate;
            break;
        }
    }

    #ifdef GNSS_DEBUG
    DEBUG_PORT.print("GNSSComputer:ConfigureDevice: Changed nav rate to ");
    DEBUG_PORT.print(navRate);
    DEBUG_PORT.println("Hz");
    #endif

    delay(pauseBetweenTasks);

    isConfigured = true;
    return true;
}


void GNSSComputer::SendUBXConfigMessage(const char *msg, size_t len)
{
    // Send over userSerial port
    gpsSerial->write(msg, len);
    
    // Wait a bit after sending config command to the device
    delay(50);
}

