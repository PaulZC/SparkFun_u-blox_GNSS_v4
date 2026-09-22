/*
  nmeaGNS.h

  Definitions for the National Marine Electronics Association standard NMEA 0183
  GNS message: GNSS fix data

  The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
  https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf

  posMode is one mode character per constellation (e.g. "AAN"), so it is
  nmeaDataTypeString rather than nmeaDataTypeChar.

*/

#pragma once

#include "../nmeaMessage.h"

class nmeaGNS : public nmeaMessage
{
public:
    const char msgId[4] = "GNS"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters
    const uint8_t messageLength = NMEA_GNS_MAX_LENGTH;

    static const uint8_t numFields = 14;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_GNS_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_GNS_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_GNS_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_GNS_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxGNS", nmeaDataTypeString, 0},
        {"time", nmeaDataTypeTime, 1},
        {"lat", nmeaDataTypeDDMM, 2},
        {"NS", nmeaDataTypeChar, 3},
        {"lon", nmeaDataTypeDDDMM, 4},
        {"EW", nmeaDataTypeChar, 5},
        {"posMode", nmeaDataTypeString, 6},
        {"numSV", nmeaDataTypeNumeric, 7},
        {"HDOP", nmeaDataTypeNumeric, 8},
        {"alt", nmeaDataTypeNumeric, 9},
        {"sep", nmeaDataTypeNumeric, 10},
        {"diffAge", nmeaDataTypeNumeric, 11},
        {"diffStation", nmeaDataTypeNumeric, 12},
        {"navStatus", nmeaDataTypeChar, 13},
    };

    /**
     * @brief Construct a new nmeaGNS object and register it with the NMEA message registry.
     *
     * NMEA GNS: GNSS fix data. Registers this message's identifier, field table, callback-copy count and
     * per-port output-enable keys with the base nmeaMessage class via addNMEA(), making it
     * discoverable by the generic NMEA dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    nmeaGNS(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaGNS);
