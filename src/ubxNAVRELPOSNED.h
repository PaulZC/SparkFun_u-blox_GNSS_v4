/*
  ubxNAVRELPOSNED.h

  v4 scaffolding: UBX-NAV-RELPOSNED - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_RELPOSNED_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "ubxMessage.h"
#include "u-blox_config_keys.h"
#include "u-blox_Class_and_ID.h"
#include "u-blox_structs.h" // For UBX_NAV_RELPOSNED_LEN_F9

class ubxNAVRELPOSNED : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_RELPOSNED;

    const char classStr[4] = "NAV";
    const char idStr[10] = "RELPOSNED";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_RELPOSNED_LEN_F9;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 27;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_RELPOSNED_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_RELPOSNED_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_RELPOSNED_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_RELPOSNED_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"refStationId", ubxDataType8bit(UBX_CFG_U2), 2, -1, -1},
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"relPosN", ubxDataType8bit(UBX_CFG_I4), 8, -1, -1},
        {"relPosE", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"relPosD", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"relPosLength", ubxDataType8bit(UBX_CFG_I4), 20, -1, -1},
        {"relPosHeading", ubxDataType8bit(UBX_CFG_I4), 24, -1, -1},
        {"relPosHPN", ubxDataType8bit(UBX_CFG_I1), 32, -1, -1},
        {"relPosHPE", ubxDataType8bit(UBX_CFG_I1), 33, -1, -1},
        {"relPosHPD", ubxDataType8bit(UBX_CFG_I1), 34, -1, -1},
        {"relPosHPLength", ubxDataType8bit(UBX_CFG_I1), 35, -1, -1},
        {"accN", ubxDataType8bit(UBX_CFG_U4), 36, -1, -1},
        {"accE", ubxDataType8bit(UBX_CFG_U4), 40, -1, -1},
        {"accD", ubxDataType8bit(UBX_CFG_U4), 44, -1, -1},
        {"accLength", ubxDataType8bit(UBX_CFG_U4), 48, -1, -1},
        {"accHeading", ubxDataType8bit(UBX_CFG_U4), 52, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X4), 60, -1, -1},
        {"gnssFixOK", ubxDataType8bit(UBX_CFG_L), 60, 0, 1},
        {"diffSoln", ubxDataType8bit(UBX_CFG_L), 60, 1, 1},
        {"relPosValid", ubxDataType8bit(UBX_CFG_L), 60, 2, 1},
        {"carrSoln", ubxDataType8bit(UBX_CFG_U1), 60, 3, 2},
        {"isMoving", ubxDataType8bit(UBX_CFG_L), 60, 5, 1},
        {"refPosMiss", ubxDataType8bit(UBX_CFG_L), 60, 6, 1},
        {"refObsMiss", ubxDataType8bit(UBX_CFG_L), 60, 7, 1},
        {"relPosHeadingValid", ubxDataType8bit(UBX_CFG_L), 60, 8, 1},
        {"relPosNormalized", ubxDataType8bit(UBX_CFG_L), 60, 9, 1}};

    ubxNAVRELPOSNED(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};
