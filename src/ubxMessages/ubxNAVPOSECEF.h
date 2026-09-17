/*
  ubxNAVPOSECEF.h

  v4 scaffolding: UBX-NAV-POSECEF - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_POSECEF_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_POSECEF_LEN

class ubxNAVPOSECEF : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_POSECEF;

    const char classStr[4] = "NAV";
    const char idStr[8] = "POSECEF";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_POSECEF_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 5;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_POSECEF_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_POSECEF_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_POSECEF_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_POSECEF_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"ecefX", ubxDataType8bit(UBX_CFG_I4), 4, -1, -1},
        {"ecefY", ubxDataType8bit(UBX_CFG_I4), 8, -1, -1},
        {"ecefZ", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"pAcc", ubxDataType8bit(UBX_CFG_U4), 16, -1, -1}};

    ubxNAVPOSECEF(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVPOSECEF);
