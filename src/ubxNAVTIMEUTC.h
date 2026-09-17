/*
  ubxNAVTIMEUTC.h

  v4 scaffolding: UBX-NAV-TIMEUTC - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_TIMEUTC_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "ubxMessage.h"
#include "u-blox_config_keys.h"
#include "u-blox_Class_and_ID.h"
#include "u-blox_structs.h" // For UBX_NAV_TIMEUTC_LEN

class ubxNAVTIMEUTC : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_TIMEUTC;

    const char classStr[4] = "NAV";
    const char idStr[8] = "TIMEUTC";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_TIMEUTC_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 14;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_TIMEUTC_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_TIMEUTC_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_TIMEUTC_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_TIMEUTC_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"tAcc", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"nano", ubxDataType8bit(UBX_CFG_I4), 8, -1, -1},
        {"year", ubxDataType8bit(UBX_CFG_U2), 12, -1, -1},
        {"month", ubxDataType8bit(UBX_CFG_U1), 14, -1, -1},
        {"day", ubxDataType8bit(UBX_CFG_U1), 15, -1, -1},
        {"hour", ubxDataType8bit(UBX_CFG_U1), 16, -1, -1},
        {"min", ubxDataType8bit(UBX_CFG_U1), 17, -1, -1},
        {"sec", ubxDataType8bit(UBX_CFG_U1), 18, -1, -1},
        {"valid", ubxDataType8bit(UBX_CFG_X1), 19, -1, -1},
        {"validTOW", ubxDataType8bit(UBX_CFG_L), 19, 0, 1},
        {"validWKN", ubxDataType8bit(UBX_CFG_L), 19, 1, 1},
        {"validUTC", ubxDataType8bit(UBX_CFG_L), 19, 2, 1},
        {"utcStandard", ubxDataType8bit(UBX_CFG_U1), 19, 4, 4}};

    ubxNAVTIMEUTC(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVTIMEUTC);
