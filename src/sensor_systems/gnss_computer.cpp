




#include "sensor_systems/gnss_computer.h"

// AltMSLParser(NMEAParser, "GNGGA", 9),
// HDOPParser(NMEAParser, "GNGSA", 16),
GNSSComputer::GNSSComputer(TwoWire *userWire)
// :GeoidSepParser(NMEAParser, "GNGGA", 11),
// PDOPParser(NMEAParser, "GNGSA", 15)
// VDOPParser(NMEAParser, "GNGSA", 17),
// TrueTrackParser(NMEAParser, "GNVTG", 1),
// GroundSpeedParser(NMEAParser, "GNVTG", 5)
{
    isConfigured = false;
    gpsWire = userWire;
}


// ----------------------------------------------------------------------------
// ConfigureDevice(GNSSNetworks_t userNetwork, GNSSDynamics_t userDynModel,
//     GNSSNavRate_t userODR)
// ----------------------------------------------------------------------------
/**
 * Configure the GPS sensor. Sends config. commands over I2C. Configures port 
 * settings, satellite network/s, GPS dynamic model, enables/disables NMEA 
 * messages, and sets output data rate.
 * 
 * @param userNetwork   Network to connect to. GPS, GLONASS, or GPS+GLONASS
 * @param userDynModel  GPS's internal fusion algorithm dynamic model. Pedestrian, 
 * portable, and airborne <1G.                     
 * @param userODR   [Hz] Output navigation rate. 5Hz or 10Hz
 * @returns True if successfully configured, false if not
 */
bool GNSSComputer::ConfigureDevice(
    GNSSNetworks_t userNetwork, GNSSDynamics_t userDynModel,
    GNSSNavRate_t userODR)
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

    gpsWire->begin();
    delay(250);  // Wait a sec
    // gpsWire->flush();

    gpsWire->beginTransmission(GNSS_I2C_ADDR);
    gpsWire->write(0xFF);

    // Check to see if the sensor ack'd
    if (gpsWire->endTransmission(false) != 0)
    {
        #ifdef GNSS_DEBUG
        DEBUG_PORT.println("GNSSComputer::ConfigureDevice ERROR: GPS did not respond");
        #endif
        return false;
    }


    // Set port settings
    SendUBXConfigMessage(UBX_CFG_PRT_I2C, UBX_CFG_PRTLEN);
    #ifdef GNSS_DEBUG
    DEBUG_PORT.println("GNSSComputer::ConfigureDevice: Changed I2C port settings.");
    #endif

    delay(pauseBetweenTasks);  // Wait a sec


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


    /* Config/disable NMEA messages */
    SendUBXConfigMessage(UBX_CFG_MSG_DISABLE_GLL, 16);
    delay(pauseBetweenTasks);

    SendUBXConfigMessage(UBX_CFG_MSG_DISABLE_GSV, 16);
    delay(pauseBetweenTasks);

    // SendUBXConfigMessage(UBX_CFG_MSG_DISABLE_RMC, 16);
    // delay(pauseBetweenTasks);
    
    // Disable GxGSA
    uint8_t cfg1[16] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x01,0x01,0x01,0x00,0x04,0x3A};
    SendUBXConfigMessage(cfg1, 16);
    delay(pauseBetweenTasks);

    // Disable GxVTG
    uint8_t cfg2[16] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x01,0x01,0x01,0x00,0x07,0x4F};
    SendUBXConfigMessage(cfg2, 16);
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
    // dataPollWait = 1000UL / (((uint32_t)navRate) * 4UL);
    dataPollWait = 20UL;
    delay(pauseBetweenTasks);
    #ifdef GNSS_DEBUG
    DEBUG_PORT.print("GNSSComputer::ConfigureDevice: Changed nav rate to ");
    DEBUG_PORT.print(navRate);
    DEBUG_PORT.print("Hz, poll time: ");
    DEBUG_PORT.print(dataPollWait);
    DEBUG_PORT.println("ms");
    // DEBUG_PORT.println("Hz");
    #endif

    isConfigured = true;
    return true;
}


// ----------------------------------------------------------------------------
// GNSSComputer::WaitForSatellites(uint32_t minSats)
// ----------------------------------------------------------------------------
/**
 * Wait for satellites to be received by the GPS. If the min. number of 
 * satellies are not aquired by a certain time period, the function returns 
 * false. Also contains aquisition checks.
 * 
 * @param minSats  Minimum number of satellites to signal a valiid fix.
 * @returns  True if success, false if not
 */
bool GNSSComputer::WaitForSatellites(uint32_t minSats)
{
    char b;
    uint32_t nSats;
    uint32_t startMillis;
    uint32_t currMillis;
    uint16_t enoughSatsCounter;

    #ifdef GNSS_DEBUG
    uint32_t prevPrint;
    uint32_t currPrint;
    uint32_t printdt;
    prevPrint = 0;
    currPrint = millis();
    printdt = (uint32_t)(navTs * 1000.0f);
    DEBUG_PORT.println("GNSSComputer::WaitForSatellites: Waiting for SVs.");
    DEBUG_PORT.print("    SVs: ");
    #endif

    // If we aren't configured, configure the GPS
    if (isConfigured == false)
    {
        if (!ConfigureDevice())
            return false;
    }

    // We will wait a minute or so to acquire a sufficient number of
    // GNSS satellite signals to ensure a good fix.
    nSats = 0;
    enoughSatsCounter = 0;
    startMillis = millis();
    currMillis = millis();
    while (currMillis - startMillis <= GNSS_POS_LOCK_TIMEOUT)
    {
        currMillis = millis();

        while (GPS_I2C.available())
        {
            b = GPS_I2C.read();
            NMEAParser.encode(b);
        }
    
        // if (NMEAParser.satellites.isUpdated() && NMEAParser.satellites.isValid())
        if (NMEAParser.satellites.isUpdated())
        {
            nSats = NMEAParser.satellites.value();  // Parse GxGGA message

            #ifdef GNSS_DEBUG
            DEBUG_PORT.print(nSats);
            DEBUG_PORT.print(",");
            #endif
            
            // We must be connected to enough sats. for a few readings to
            // ensure a good fix
            if (nSats >= minSats)
                enoughSatsCounter++;
        }

        // Print status to debug port
        #ifdef GNSS_DEBUG
        currPrint = millis();
        if (currPrint - prevPrint >= printdt)
        {
            DEBUG_PORT.print(nSats);
            DEBUG_PORT.print(",");
            prevPrint = currPrint;
        }
        #endif

        // If we've acquired enough sats. for at least 5 nav. updates,
        // it is safe to say that we are reliably connected to enough sats.
        if (enoughSatsCounter >= 5)
        {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println(" Sufficient SVs. acq.");
            #endif
            return true;
        }

    }

    // If our code gets to this point, we've waited too long to connect to 
    // enough sats. Something might not be right or sat. orbits aren't in our 
    // favor :(
    #ifdef GNSS_DEBUG
    DEBUG_PORT.println();
    DEBUG_PORT.print("GNSSComputer::WaitForSatellites ERROR: Took too long to find ");
    DEBUG_PORT.print(minSats);
    DEBUG_PORT.println(" SVs.");
    #endif

    return false;
}


// ----------------------------------------------------------------------------
// GNSSComputer::ListenForData()
// ----------------------------------------------------------------------------
/**
 * Read data from GPS I2C port and feed the TinyGPS NMEA parser data. Also 
 * includes checks to ensure data integrity.
 * 
 * @returns True if success, false if not.
 */
// https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library/blob/1e70755453a898d8267ced641500c33d377409b7/src/SparkFun_Ublox_Arduino_Library.cpp#L347
bool GNSSComputer::ListenForData()
{
    // Put statement at the top of the code and allocate variables within it
    if (millis() - lastDataCheck >= dataPollWait)
    {
        /* Get the bytes available for read */
        uint8_t hibyte;
        uint8_t lobyte;
        uint8_t byteFromGps;
        uint16_t i;
        uint16_t bytesAvail;
        uint16_t bytesToRead;

        bytesAvail = 0;

        // Check for connection
        gpsWire->beginTransmission(GNSS_I2C_ADDR);
        gpsWire->write(0xFD);  // See p. 38 of u-blox M8 Receiver description
        if (gpsWire->endTransmission(false) != 0)
        {
            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ListenForData ERROR: GPS did not respond");
            #endif
            return false;
        }

        gpsWire->requestFrom(GNSS_I2C_ADDR, (uint8_t)2);  // req. hi and lo byte

        if (gpsWire->available())
        {
            hibyte = gpsWire->read();
            lobyte = gpsWire->read();

            // Check for possible ublox bug presented in Sparkfun code
            if (lobyte == 0xFF)
            {
                #ifdef GNSS_DEBUG
                DEBUG_PORT.println("GNSSComputer::ListenForData ERROR: Encountered lsb=0xFF bug");
                #endif
                lastDataCheck = millis();
                return false;
            }

            bytesAvail = (uint16_t)hibyte << 8 | lobyte;  // Cvt. to 16-bit value
        }

        // Check if empty
        if (bytesAvail == 0)
        {
            // #ifdef GNSS_DEBUG
            // DEBUG_PORT.println("GNSSComputer::ListenForData No bytes available, ok");
            // #endif
            lastDataCheck = millis();
            return false;
        }

        // Check for "bytes available" error documented in Sparkfun's code
        if (bytesAvail & ((uint16_t)1 << 15))
        {
            // Clear hibyte
            bytesAvail &= ~((uint16_t)1 << 15);

            #ifdef GNSS_DEBUG
            DEBUG_PORT.println("GNSSComputer::ListenForData ERROR: Encountered bytes available error");
            #endif
        }

        /* Read the data */
        // Yeah, while loops are bad, but whatevs
        // bytesAvail decreases as bytes are read. The loop will terminate when all 
        // bytes are read (bytesAvail = 0)
        bytesToRead = 0;
        while (bytesAvail)
        {
            // Check connection
            gpsWire->beginTransmission(GNSS_I2C_ADDR);
            gpsWire->write(0xFF);
            if (gpsWire->endTransmission(false) != 0)
            {
                #ifdef GNSS_DEBUG
                DEBUG_PORT.println("GNSSComputer::ListenForData ERROR: GPS did not respond");
                #endif
                return false;
            }

            /**
             * Limit bytes to read to device I2C buffer size, 32 bytes for 
             * Arduino boards and Teensy 4.1
             * 
             * bytesToRead = bytesAvail;
             * if (bytesToRead > 32)
             *     bytesToRead = 32;
             */
            bytesToRead = (bytesAvail > GNSS_I2C_BUFFSIZE) ? GNSS_I2C_BUFFSIZE : bytesAvail;
        
            TRY_AGAIN:  // Checkpoint for when we encounter the 0x7F thingy

            // Grab some bytes to eat
            gpsWire->requestFrom(GNSS_I2C_ADDR, (uint8_t)bytesToRead);
            if (gpsWire->available())
            {
                for (i = 0; i < bytesToRead; i++)
                {
                    byteFromGps = gpsWire->read();

                    // Check for 0x7F error-thing outlined in Sparkfun's code
                    // I'm not sure if we need to check for this, but I put in in, just
                    // in case. Otherwise, we can comment it out
                    if (i == 0 && byteFromGps == 0x7F)
                    {
                        #ifdef GNSS_DEBUG
                        DEBUG_PORT.println("GNSSComputer::ListenForData WARNING: Encountered 0x7F error");
                        #endif

                        // This delay() probs isn't good to have in final flight code, but we might need it if this 
                        // error is frequent
                        delay(3);  // Sparkfun has 5ms, imma use 3ms
                        goto TRY_AGAIN;
                    }

                    /* Pass the received byte on to TinyGPS to form and parse NMEA data */
                    NMEAParser.encode((char)byteFromGps);
                }
            }
            else
            {
                // Sensor was not available/didn't respond
                return false;
            }

            bytesAvail -= bytesToRead;  // Decrease counter
        }
    }

    // If we get to this point, we've received valid bytes!
    return true;
}
















// ----------------------------------------------------------------------------
// GNSSComputer::SendUBXConfigMessage(const uint8_t *msg, size_t len)
// ----------------------------------------------------------------------------
/**
 * Send UBX configuration message over I2C.
 * 
 * @param msg  uint8_t array of bytes to send.
 * @param  len  Length of message
 */
void GNSSComputer::SendUBXConfigMessage(const uint8_t *msg, size_t len)
{
    gpsWire->beginTransmission(GNSS_I2C_ADDR);
    gpsWire->write(0xFF);

    // Check to see if the sensor ack'd
    if (gpsWire->endTransmission(false) != 0)
    {
        #ifdef GNSS_DEBUG
        DEBUG_PORT.println("GNSSComputer::SendUBXConfigMessage ERROR: GPS did not respond to request");
        #endif
        // return GNSS_I2C_COMM_ERROR;
    }

    // Send message
    gpsWire->beginTransmission(GNSS_I2C_ADDR);
    gpsWire->write(msg, len);
    
    // Release the bus
    if (gpsWire->endTransmission() != 0)
    {
        #ifdef GNSS_DEBUG
        DEBUG_PORT.println("GNSSComputer::SendUBXConfigMessage ERROR: GPS did not release after message");
        #endif
    }

    // Wait a bit
    delay(50);
}

// // Serial Version
// void GNSSComputer::SendUBXConfigMessage(const uint8_t *msg, size_t len)
// {
//     // Send over userSerial port
//     GPS_PORT.write(msg, len);
    
//     // Wait a bit after sending config command to the device
//     delay(50);
// }


/* GetInstance() function for singleton */
GNSSComputer &GNSSComputer::GetInstance()
{
    static GNSSComputer instance;
    return instance;
}

GNSSComputer &GPS = GPS.GetInstance();
