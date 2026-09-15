/*
  ubxRXMCOR.h

  v4 scaffolding: UBX-RXM-COR - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_RXM_COR_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "ubxMessage.h"
#include "u-blox_config_keys.h"
#include "u-blox_Class_and_ID.h"
#include "u-blox_structs.h" // For UBX_RXM_COR_LEN

class ubxRXMCOR : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_RXM;
    const uint8_t ID = UBX_RXM_COR;

    const char classStr[4] = "RXM";
    const char idStr[4] = "COR";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_RXM_COR_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 14;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_RXM_COR_I2C, UBLOX_CFG_MSGOUT_UBX_RXM_COR_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_RXM_COR_UART1, UBLOX_CFG_MSGOUT_UBX_RXM_COR_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"ebno", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},
        {"statusInfo", ubxDataType8bit(UBX_CFG_X4), 4, -1, -1},
        {"protocol", ubxDataType8bit(UBX_CFG_U1), 4, 0, 5},
        {"errStatus", ubxDataType8bit(UBX_CFG_U1), 4, 5, 2},
        {"msgUsed", ubxDataType8bit(UBX_CFG_U1), 4, 7, 2},
        {"correctionId", ubxDataType8bit(UBX_CFG_U1), 4, 9, 16},
        {"msgTypeValid", ubxDataType8bit(UBX_CFG_L), 4, 25, 1},
        {"msgSubTypeValid", ubxDataType8bit(UBX_CFG_L), 4, 26, 1},
        {"msgInputHandle", ubxDataType8bit(UBX_CFG_L), 4, 27, 1},
        {"msgEncrypted", ubxDataType8bit(UBX_CFG_U1), 4, 28, 2},
        {"msgDecrypted", ubxDataType8bit(UBX_CFG_U1), 4, 30, 2},
        {"msgType", ubxDataType8bit(UBX_CFG_U2), 8, -1, -1},
        {"msgSubType", ubxDataType8bit(UBX_CFG_U2), 10, -1, -1}};

    ubxRXMCOR(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};
