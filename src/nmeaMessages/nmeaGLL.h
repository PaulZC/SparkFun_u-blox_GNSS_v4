/*
  nmeaGLL.h

  Definitions for the National Marine Electronics Association standard NMEA 0183
  GLL message: Latitude and longitude, with time of position fix and status

  The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
  https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf

*/

#pragma once

#include "../nmeaMessage.h"

class nmeaGLL : public nmeaMessage
{
public:
    const char msgId[4] = "GLL"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters
    const uint8_t messageLength = NMEA_GLL_MAX_LENGTH;

    static const uint8_t numFields = 8;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_GLL_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_GLL_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_GLL_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_GLL_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxGLL", nmeaDataTypeString, 0},
        {"lat", nmeaDataTypeDDMM, 1},
        {"NS", nmeaDataTypeChar, 2},
        {"lon", nmeaDataTypeDDDMM, 3},
        {"EW", nmeaDataTypeChar, 4},
        {"time", nmeaDataTypeTime, 5},
        {"status", nmeaDataTypeChar, 6},
        {"posMode", nmeaDataTypeChar, 7},
    };

    nmeaGLL(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaGLL);
