


/**
 * Teensy 4.1 has a 64 byte serial buffer that can be expanded with:
 *      Serial1.addMemoryForRead(void *buffer, size_t size)
 */



#pragma once



#include <Arduino.h>
#include "hummingbird_config.h"
#include "maths/vectors.h"



/* Possible baud rates for the GNSS sensor */
typedef enum
{
    GNSS_BAUD_9600,     // 9600 serial baud rate (default when NEO-M8N powers up)
    GNSS_BAUD_38400,    // 38.4k serial baud rate
    GNSS_BAUD_115200,   // 115.2K serial baud rate
    GNSS_BAUD_230400    // 230.4k serial baud rate
} GNSSBaudRates_t;


/* Possible GNSS networks to connect to. */
typedef enum
{
    GNSS_NET_GPS,           // Connect to GPS network, max. 10Hz on NEO-M8N
    GNSS_NET_GLONASS,       // Connect to GLONASS network, max. 10Hz on NEO-M8N
    GNSS_NET_GPS_GLONASS    // Connect to both GPS and GLONASS, max. 5Hz on NEO-M8N
} GNSSNetworks_t;


/* Various dynamic models for the GPS receiver's sensor fusion algorithms */
typedef enum
{
    GNSS_DYNAMICS_Portable,     // Portable dynamic model, 12000m alt, 310m/s hvel, 50m/s vvel, med. dev.
    GNSS_DYNAMICS_Pedestrian,   // Pedestrian dynamic model, 9000m alt, 30m/s hvel, 20m/s vvel, small dev.
    GNSS_DYNAMICS_Airborne1G    // Airborne 1G dynamic model, 50000m alt, 100m/s hvel, 100m/s vvel, large dev.
} GNSSDynamics_t;


/* GPS fix status codes */
typedef enum
{
    GNSS_FIX_NO_GPS,    // No GPS sensor connected/detected
    GNSS_FIX_NONE,      // Receiving valid GPS data, but with no position lock
    GNSS_FIX_2D,        // Receiving valid GPS data with a 2D lock
    GNSS_FIX_3D         // Receiving valid GPS data with a 3D lock
} GNSSFix_t;

/* UBX Configuration char Arrays */





class GNSSSensor
{
public:
    GNSSSensor(HardwareSerial *userSerial = &GPS_PORT);
protected:
private:
    GNSSBaudRates_t gpsBaud;        // Baud rate for GPS serial communication
    GNSSNetworks_t network;         // Satellite network(s) that the GPS is connected to
    GNSSDynamics_t dynamicModel;    // Dynamic model for the GPS fusion algorithms
    HardwareSerial *gpsSerial;      // HW serial port that the GPS is connected to
    Vectord PosLLA;     // [rad, rad, m] Lat, lon, altitude
    Vectorf PosECEF;    // [m, m, m] ECEF position
    Vectorf VelECEF;    // [m/s, m/s, m/s] ECEF velocity

};