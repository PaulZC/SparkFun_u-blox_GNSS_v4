/*
  ubxESFINS.h

  v4 scaffolding: UBX-ESF-INS - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_ESF_INS_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_ESF_INS_LEN

class ubxESFINS : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_ESF;
    const uint8_t ID = UBX_ESF_INS;

    const char classStr[4] = "ESF";
    const char idStr[4] = "INS";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_ESF_INS_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 15;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_ESF_INS_I2C, UBLOX_CFG_MSGOUT_UBX_ESF_INS_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_ESF_INS_UART1, UBLOX_CFG_MSGOUT_UBX_ESF_INS_UART2};

    const ubxField ubxFields[numFields] = {
        {"bitfield0", ubxDataType8bit(UBX_CFG_X4), 0, -1, -1},
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, 0, 8},
        {"xAngRateValid", ubxDataType8bit(UBX_CFG_L), 0, 8, 1},
        {"yAngRateValid", ubxDataType8bit(UBX_CFG_L), 0, 9, 1},
        {"zAngRateValid", ubxDataType8bit(UBX_CFG_L), 0, 10, 1},
        {"xAccelValid", ubxDataType8bit(UBX_CFG_L), 0, 11, 1},
        {"yAccelValid", ubxDataType8bit(UBX_CFG_L), 0, 12, 1},
        {"zAccelValid", ubxDataType8bit(UBX_CFG_L), 0, 13, 1},
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"xAngRate", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"yAngRate", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"zAngRate", ubxDataType8bit(UBX_CFG_I4), 20, -1, -1},
        {"xAccel", ubxDataType8bit(UBX_CFG_I4), 24, -1, -1},
        {"yAccel", ubxDataType8bit(UBX_CFG_I4), 28, -1, -1},
        {"zAccel", ubxDataType8bit(UBX_CFG_I4), 32, -1, -1}};

    ubxESFINS(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxESFINS);
