/*
  nmeaGST.h

  Definitions for the National Marine Electronics Association standard NMEA 0183
  GST message: GNSS Pseudo Range Error Statistics

  The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
  https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf

*/

#pragma once

#include "../nmeaMessage.h"

class nmeaGST : public nmeaMessage
{
public:
    const char msgId[4] = "GST"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters
    const uint8_t messageLength = 100;

    static const uint8_t numFields = 9;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_GST_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_GST_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_GST_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_GST_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxGST", nmeaDataTypeString, 0},
        {"time", nmeaDataTypeTime, 1},
        {"rangeRms", nmeaDataTypeNumeric, 2},
        {"stdMajor", nmeaDataTypeNumeric, 3},
        {"stdMinor", nmeaDataTypeNumeric, 4},
        {"orient", nmeaDataTypeNumeric, 5},
        {"stdLat", nmeaDataTypeNumeric, 6},
        {"stdLon", nmeaDataTypeNumeric, 7},
        {"stdAlt", nmeaDataTypeNumeric, 8},
    };

    nmeaGST(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaGST);
