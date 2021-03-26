


/**
 * Teensy 4.1 has a 64 byte serial buffer that can be expanded with:
 *      Serial1.addMemoryForRead(void *buffer, size_t size)
 */



#pragma once



#include <Arduino.h>
#include <Wire.h>
#include "hummingbird_config.h"
#include "maths/vectors.h"
#include "TinyGPS++.h"
#include "constants.h"
#include "sensor_systems/ubx_cfg_messages.h"


#if defined(DEBUG) && defined(DEBUG_PORT)
    /* Print GNSS debug messages to the debug serial port */
    #define GNSS_DEBUG
#endif




/** 
 * [ms] millis() How long we will wait for the GPS to get a position lock and connect to at 
 * least N sats. 
 */
constexpr uint32_t GNSS_POS_LOCK_TIMEOUT = 120000;

/* Baud rate that the unconfigured GPS has when powered up */
constexpr int32_t GNSS_DEFAULT_BAUD = 9600;

/* Minimum number of GNSS satellites to ensure a valid fix */
constexpr uint32_t GNSS_MIN_SATS = 5;

/* I2C address of the UBLOX GPS module */
constexpr uint8_t GNSS_I2C_ADDR = 0x42;


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
    GNSS_DYNAMICS_PORTABLE,     // Portable dynamic model, 12000m alt, 310m/s hvel, 50m/s vvel, med. dev.
    GNSS_DYNAMICS_PEDESTRIAN,   // Pedestrian dynamic model, 9000m alt, 30m/s hvel, 20m/s vvel, small dev.
    GNSS_DYNAMICS_AIRBORNE_1G    // Airborne 1G dynamic model, 50000m alt, 100m/s hvel, 100m/s vvel, large dev.
} GNSSDynamics_t;

/* GPS fix status codes */
typedef enum
{
    // GNSS_FIX_NO_GPS,    // No GPS sensor connected/detected
    GNSS_FIX_NONE,      // Receiving valid GPS data, but with no position lock
    GNSS_FIX_2D,        // Receiving valid GPS data with a 2D lock
    GNSS_FIX_3D         // Receiving valid GPS data with a 3D lock
} GNSSFix_t;

/* GPS inavigation rates */
typedef enum
{
    GNSS_NAVRATE_5HZ = 5,   // 5 Hz navigation rate/output rate
    GNSS_NAVRATE_10HZ = 10  // 10 Hz navigation rate/output rate
} GNSSNavRate_t;


/* GPS configuration error types */
typedef enum
{
    GNSS_CONFIG_SUCCESS,  // Successful configuration
    GNSS_CONFIG_NO_DEVICE,  // No GPS sensor connected/detected
    GNSS_CONFIG_RECONNECT_ERROR,  // When we change the baud rate and cannot reconnect to the GPS
    GNSS_CONFIG_NO_HOME_LOCATION  // No home location specified
} GNSSConfig_t;



class GNSSComputer
{
public:
    GNSSComputer(
        TwoWire *userWire = &GPS_I2C,
        GNSSBaudRates_t initialBaud = GNSS_BAUD_115200
        );
    ~GNSSComputer() {};

    // Do not allow copies (singleton)
    static GNSSComputer &GetInstance();
    GNSSComputer(const GNSSComputer &) = delete;
    GNSSComputer &operator=(const GNSSComputer &) = delete;

    bool ConfigureDevice(
        GNSSBaudRates_t userBaud    = GNSS_BAUD_115200, 
        GNSSNetworks_t userNetwork  = GNSS_NET_GPS_GLONASS, 
        GNSSDynamics_t userDynModel = GNSS_DYNAMICS_PEDESTRIAN,
        GNSSNavRate_t userODR       = GNSS_NAVRATE_5HZ
        );
    bool WaitForSatellites(uint32_t nSats = GNSS_MIN_SATS);
    bool ListenForData();

protected:
private:
    TinyGPSPlus NMEAParser;          // TinyGPS++ GPS object
    // GGA
    // ortho. height
    // geoid sep.
    // gps quality/fix type?
    // TinyGPSCustom AltMSLParser;  // Parse GxGGA for orthometric alt [m] (alt MSL)
    TinyGPSCustom GeoidSepParser;  // Parse GxGGA for geoid separation [m] (Sep = AltWGS84 - AltMSL)

    // GSA
    // pdop
    // hdop
    // vdop
    TinyGPSCustom PDOPParser;  // Parse GxGSA for PDOP
    // TinyGPSCustom HDOPParser;  // Parse GxGSA for HDOP
    TinyGPSCustom VDOPParser;  // Parse GxGSA for VDOP

    // VTG
    // mag. track/cog
    // true track/cog
    // speed, kts
    // speed over gnd, kph
    TinyGPSCustom TrueTrackParser;  // Parse GxVTG for true track [deg]
    TinyGPSCustom GroundSpeedParser;  // Parse GxVTG for ground speed [kts]

    Vectord PosLLA;     // [rad, rad, m] Lat, lon, altitude
    Vectorf PosECEF;    // [m, m, m] ECEF position
    Vectorf VelECEF;    // [m/s, m/s, m/s] ECEF velocity
    void SendUBXConfigMessage(const uint8_t *msg, size_t len);
    // LPF for smoothing gps pos (to smooth out impulses/sharp changes)
    // LPF for smoothing altitude
private:
    bool isConfigured;  // True if ConfigureDevice() was called, false if not
    uint32_t lastDataCheck;  // [ms] millis() of when last checked for new data.
    uint32_t dataPollWait;  // [ms] Period between polling for new data. 
    int32_t gpsBaud;  // Baud rate for serial connection
    float navTs;  // [sec] GPS navigation sample period
    float navRate;  // [Hz] GPS navigation rate
    

    // GNSSBaudRates_t gpsBaud;        // Baud rate for GPS serial communication
    GNSSNetworks_t network;         // Satellite network(s) that the GPS is connected to
    GNSSDynamics_t dynamicModel;    // Dynamic model for the GPS fusion algorithms
    GNSSNavRate_t updateRate;       // [Hz] navigation rate from gps
    TwoWire *gpsWire;      // HW I2C bus that the GPS is connected to
};

// Only one instance
extern GNSSComputer &GPS;