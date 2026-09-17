/*
  nmeaVTG.h

  Definitions for the National Marine Electronics Association standard NMEA 0183
  VTG message: Course over ground and Ground speed

  The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
  https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf

*/

#pragma once

#include "../nmeaMessage.h"

class nmeaVTG : public nmeaMessage
{
public:
    const char msgId[4] = "VTG"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters
    const uint8_t messageLength = 50;

    static const uint8_t numFields = 10;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_VTG_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_VTG_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_VTG_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_VTG_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxVTG", nmeaDataTypeString, 0},
        {"cogt", nmeaDataTypeNumeric, 1},
        {"cogtUnit", nmeaDataTypeChar, 2},
        {"cogm", nmeaDataTypeNumeric, 3},
        {"cogmUnit", nmeaDataTypeChar, 4},
        {"sogn", nmeaDataTypeNumeric, 5},
        {"sognUnit", nmeaDataTypeChar, 6},
        {"sogk", nmeaDataTypeNumeric, 7},
        {"sogkUnit", nmeaDataTypeChar, 8},
        {"posMode", nmeaDataTypeChar, 9},
    };

    nmeaVTG(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaVTG);
