/*
  ubxNAVDOP.h

  v4 scaffolding: UBX-NAV-DOP - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_DOP_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_DOP_LEN

class ubxNAVDOP : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_DOP;

    const char classStr[4] = "NAV";
    const char idStr[4] = "DOP";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_DOP_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 8;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_DOP_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_DOP_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_DOP_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_DOP_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"gDOP", ubxDataType8bit(UBX_CFG_U2), 4, -1, -1},
        {"pDOP", ubxDataType8bit(UBX_CFG_U2), 6, -1, -1},
        {"tDOP", ubxDataType8bit(UBX_CFG_U2), 8, -1, -1},
        {"vDOP", ubxDataType8bit(UBX_CFG_U2), 10, -1, -1},
        {"hDOP", ubxDataType8bit(UBX_CFG_U2), 12, -1, -1},
        {"nDOP", ubxDataType8bit(UBX_CFG_U2), 14, -1, -1},
        {"eDOP", ubxDataType8bit(UBX_CFG_U2), 16, -1, -1}};

    ubxNAVDOP(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVDOP);
