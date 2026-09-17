/*
  nmeaGGA.h

  Definitions for the National Marine Electronics Association standard NMEA 0183
  GGA message: Global Positioning System Fix Data

  The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
  https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf

*/

#pragma once

#include "../nmeaMessage.h"

class nmeaGGA : public nmeaMessage
{
public:
    const char msgId[4] = "GGA"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters, but GNSS output can include additional decimal places
    const uint8_t messageLength = 100;

    static const uint8_t numFields = 15;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_GGA_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_GGA_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_GGA_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_GGA_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxGGA", nmeaDataTypeString, 0},
        {"time", nmeaDataTypeTime, 1},
        {"lat", nmeaDataTypeDDMM, 2},
        {"NS", nmeaDataTypeChar, 3},
        {"lon", nmeaDataTypeDDDMM, 4},
        {"EW", nmeaDataTypeChar, 5},
        {"quality", nmeaDataTypeDigit, 6},
        {"numSV", nmeaDataTypeNumeric, 7},
        {"HDOP", nmeaDataTypeNumeric, 8},
        {"alt", nmeaDataTypeNumeric, 9},
        {"altUnit", nmeaDataTypeChar, 10},
        {"sep", nmeaDataTypeNumeric, 11},
        {"sepUnit", nmeaDataTypeChar, 12},
        {"diffAge", nmeaDataTypeNumeric, 13},
        {"diffStation", nmeaDataTypeNumeric, 14},
};

    nmeaGGA(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaGGA);
