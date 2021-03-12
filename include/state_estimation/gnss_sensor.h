






#pragma once


#include <Arduino.h>
#include "hummingbird_config.h"
#include "vector.h"


class GNSSSensor
{
public:
    GNSSSensor(HardwareSerial *userSerial = &GPS_PORT);
protected:
private:
    HardwareSerial *gpsSerial;  // HW serial port that the GPS is connected to
};