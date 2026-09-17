/*
  ubxESFALG.h

  v4 scaffolding: UBX-ESF-ALG - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_ESF_ALG_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_ESF_ALG_LEN

class ubxESFALG : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_ESF;
    const uint8_t ID = UBX_ESF_ALG;

    const char classStr[4] = "ESF";
    const char idStr[4] = "ALG";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_ESF_ALG_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 12;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_ESF_ALG_I2C, UBLOX_CFG_MSGOUT_UBX_ESF_ALG_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_ESF_ALG_UART1, UBLOX_CFG_MSGOUT_UBX_ESF_ALG_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"version", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 5, -1, -1},
        {"autoMntAlgOn", ubxDataType8bit(UBX_CFG_L), 5, 0, 1},
        {"status", ubxDataType8bit(UBX_CFG_U1), 5, 1, 3},
        {"error", ubxDataType8bit(UBX_CFG_X1), 6, -1, -1},
        {"tiltAlgError", ubxDataType8bit(UBX_CFG_L), 6, 0, 1},
        {"yawAlgError", ubxDataType8bit(UBX_CFG_L), 6, 1, 1},
        {"angleError", ubxDataType8bit(UBX_CFG_L), 6, 2, 1},
        {"yaw", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"pitch", ubxDataType8bit(UBX_CFG_I2), 12, -1, -1},
        {"roll", ubxDataType8bit(UBX_CFG_I2), 14, -1, -1}};

    ubxESFALG(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxESFALG);
