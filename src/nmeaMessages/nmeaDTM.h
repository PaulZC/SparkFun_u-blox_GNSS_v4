/*
  nmeaDTM.h

  Definitions for the National Marine Electronics Association standard NMEA 0183
  DTM message: Datum Reference

  The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
  https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf

  lat/lon here are the datum offset in minutes (numeric), not degrees+minutes,
  so they use nmeaDataTypeNumeric rather than nmeaDataTypeDDMM/DDDMM.

*/

#pragma once

#include "../nmeaMessage.h"

class nmeaDTM : public nmeaMessage
{
public:
    const char msgId[4] = "DTM"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters
    const uint8_t messageLength = 50;

    static const uint8_t numFields = 9;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_DTM_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_DTM_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_DTM_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_DTM_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxDTM", nmeaDataTypeString, 0},
        {"datum", nmeaDataTypeString, 1},
        {"subDatum", nmeaDataTypeString, 2},
        {"lat", nmeaDataTypeNumeric, 3},
        {"NS", nmeaDataTypeChar, 4},
        {"lon", nmeaDataTypeNumeric, 5},
        {"EW", nmeaDataTypeChar, 6},
        {"alt", nmeaDataTypeNumeric, 7},
        {"refDatum", nmeaDataTypeString, 8},
    };

    nmeaDTM(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaDTM);
