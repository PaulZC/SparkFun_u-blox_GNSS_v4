/*
  ubxNAVSVIN.h

  v4 scaffolding: UBX-NAV-SVIN - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_SVIN_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "ubxMessage.h"
#include "u-blox_config_keys.h"
#include "u-blox_Class_and_ID.h"
#include "u-blox_structs.h" // For UBX_NAV_SVIN_LEN

class ubxNAVSVIN : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_SVIN;

    const char classStr[4] = "NAV";
    const char idStr[5] = "SVIN";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_SVIN_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 13;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_SVIN_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_SVIN_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_SVIN_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_SVIN_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"dur", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"meanX", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"meanY", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"meanZ", ubxDataType8bit(UBX_CFG_I4), 20, -1, -1},
        {"meanXHP", ubxDataType8bit(UBX_CFG_I1), 24, -1, -1},
        {"meanYHP", ubxDataType8bit(UBX_CFG_I1), 25, -1, -1},
        {"meanZHP", ubxDataType8bit(UBX_CFG_I1), 26, -1, -1},
        {"meanAcc", ubxDataType8bit(UBX_CFG_U4), 28, -1, -1},
        {"obs", ubxDataType8bit(UBX_CFG_U4), 32, -1, -1},
        {"valid", ubxDataType8bit(UBX_CFG_I1), 36, -1, -1},
        {"active", ubxDataType8bit(UBX_CFG_I1), 37, -1, -1}};

    ubxNAVSVIN(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};
