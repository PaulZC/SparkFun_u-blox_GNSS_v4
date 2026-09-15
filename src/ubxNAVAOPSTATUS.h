/*
  ubxNAVAOPSTATUS.h

  v4 scaffolding: UBX-NAV-AOPSTATUS - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_AOPSTATUS_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "ubxMessage.h"
#include "u-blox_config_keys.h"
#include "u-blox_Class_and_ID.h"
#include "u-blox_structs.h" // For UBX_NAV_AOPSTATUS_LEN

class ubxNAVAOPSTATUS : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_AOPSTATUS;

    const char classStr[4] = "NAV";
    const char idStr[10] = "AOPSTATUS";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_AOPSTATUS_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 4;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_AOPSTATUS_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_AOPSTATUS_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_AOPSTATUS_UART1, 0};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"aopCfg", ubxDataType8bit(UBX_CFG_X1), 4, -1, -1},
        {"useAOP", ubxDataType8bit(UBX_CFG_L), 4, 0, 1},
        {"status", ubxDataType8bit(UBX_CFG_U1), 5, -1, -1}};

    ubxNAVAOPSTATUS(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};
