/*
  nmeaZDA.h

  Definitions for the National Marine Electronics Association standard NMEA 0183
  ZDA message: Time and Date

  The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
  https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf

  day/month are two-digit fields with leading zeros if needed, so they use nmeaDataTypeString.

*/

#pragma once

#include "../nmeaMessage.h"

class nmeaZDA : public nmeaMessage
{
public:
    const char msgId[4] = "ZDA"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters
    const uint8_t messageLength = NMEA_ZDA_MAX_LENGTH;

    static const uint8_t numFields = 7;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_ZDA_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_ZDA_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_ZDA_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_ZDA_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxZDA", nmeaDataTypeString, 0},
        {"time", nmeaDataTypeTime, 1},
        {"day", nmeaDataTypeString, 2}, // Use the String type so we record the leading zero correctly
        {"month", nmeaDataTypeString, 3}, // Use the String type so we record the leading zero correctly
        {"year", nmeaDataTypeNumeric, 4},
        {"ltzh", nmeaDataTypeNumeric, 5},
        {"ltzn", nmeaDataTypeNumeric, 6},
    };

    nmeaZDA(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaZDA);
