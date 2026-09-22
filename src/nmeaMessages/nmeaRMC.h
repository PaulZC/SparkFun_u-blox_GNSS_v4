/*
  nmeaRMC.h

  Definitions for the National Marine Electronics Association standard NMEA 0183
  RMC message: Recommended minimum data

  The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
  https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf

  date (ddmmyy) is kept as nmeaDataTypeString (a raw digit copy) rather than
  nmeaDataTypeNumeric, so a leading zero in the day or month is not lost.

*/

#pragma once

#include "../nmeaMessage.h"

class nmeaRMC : public nmeaMessage
{
public:
    const char msgId[4] = "RMC"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters
    const uint8_t messageLength = NMEA_RMC_MAX_LENGTH;

    static const uint8_t numFields = 14;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_RMC_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_RMC_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_RMC_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_RMC_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxRMC", nmeaDataTypeString, 0},
        {"time", nmeaDataTypeTime, 1},
        {"status", nmeaDataTypeChar, 2},
        {"lat", nmeaDataTypeDDMM, 3},
        {"NS", nmeaDataTypeChar, 4},
        {"lon", nmeaDataTypeDDDMM, 5},
        {"EW", nmeaDataTypeChar, 6},
        {"spd", nmeaDataTypeNumeric, 7},
        {"cog", nmeaDataTypeNumeric, 8},
        {"date", nmeaDataTypeString, 9},
        {"mv", nmeaDataTypeNumeric, 10},
        {"mvEW", nmeaDataTypeChar, 11},
        {"posMode", nmeaDataTypeChar, 12},
        {"navStatus", nmeaDataTypeChar, 13},
    };

    /**
     * @brief Construct a new nmeaRMC object and register it with the NMEA message registry.
     *
     * NMEA RMC: Recommended minimum data. Registers this message's identifier, field table, callback-copy count and
     * per-port output-enable keys with the base nmeaMessage class via addNMEA(), making it
     * discoverable by the generic NMEA dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    nmeaRMC(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaRMC);
