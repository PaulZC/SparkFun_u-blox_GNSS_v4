/*
  ubxNAVPVAT.h

  v4 scaffolding: UBX-NAV-PVAT - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_PVAT_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "ubxMessage.h"
#include "u-blox_config_keys.h"
#include "u-blox_Class_and_ID.h"
#include "u-blox_structs.h" // For UBX_NAV_PVAT_LEN

class ubxNAVPVAT : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_PVAT;

    const char classStr[4] = "NAV";
    const char idStr[5] = "PVAT";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_PVAT_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 51;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_PVAT_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_PVAT_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_PVAT_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_PVAT_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"version", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},
        {"valid", ubxDataType8bit(UBX_CFG_X1), 5, -1, -1},
        {"validDate", ubxDataType8bit(UBX_CFG_L), 5, 0, 1},
        {"validTime", ubxDataType8bit(UBX_CFG_L), 5, 1, 1},
        {"fullyResolved", ubxDataType8bit(UBX_CFG_L), 5, 2, 1},
        {"validMag", ubxDataType8bit(UBX_CFG_L), 5, 3, 1},
        {"year", ubxDataType8bit(UBX_CFG_U2), 6, -1, -1},
        {"month", ubxDataType8bit(UBX_CFG_U1), 8, -1, -1},
        {"day", ubxDataType8bit(UBX_CFG_U1), 9, -1, -1},
        {"hour", ubxDataType8bit(UBX_CFG_U1), 10, -1, -1},
        {"min", ubxDataType8bit(UBX_CFG_U1), 11, -1, -1},
        {"sec", ubxDataType8bit(UBX_CFG_U1), 12, -1, -1},
        {"tAcc", ubxDataType8bit(UBX_CFG_U4), 16, -1, -1},
        {"nano", ubxDataType8bit(UBX_CFG_I4), 20, -1, -1},
        {"fixType", ubxDataType8bit(UBX_CFG_U1), 24, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 25, -1, -1},
        {"gnssFixOK", ubxDataType8bit(UBX_CFG_L), 25, 0, 1},
        {"diffSoln", ubxDataType8bit(UBX_CFG_L), 25, 1, 1},
        {"vehRollValid", ubxDataType8bit(UBX_CFG_L), 25, 3, 1},
        {"vehPitchValid", ubxDataType8bit(UBX_CFG_L), 25, 4, 1},
        {"vehHeadingValid", ubxDataType8bit(UBX_CFG_L), 25, 5, 1},
        {"carrSoln", ubxDataType8bit(UBX_CFG_U1), 25, 6, 2},
        {"flags2", ubxDataType8bit(UBX_CFG_X1), 26, -1, -1},
        {"confirmedAvai", ubxDataType8bit(UBX_CFG_L), 26, 5, 1},
        {"confirmedDate", ubxDataType8bit(UBX_CFG_L), 26, 6, 1},
        {"confirmedTime", ubxDataType8bit(UBX_CFG_L), 26, 7, 1},
        {"numSV", ubxDataType8bit(UBX_CFG_U1), 27, -1, -1},
        {"lon", ubxDataType8bit(UBX_CFG_I4), 28, -1, -1},
        {"lat", ubxDataType8bit(UBX_CFG_I4), 32, -1, -1},
        {"height", ubxDataType8bit(UBX_CFG_I4), 36, -1, -1},
        {"hMSL", ubxDataType8bit(UBX_CFG_I4), 40, -1, -1},
        {"hAcc", ubxDataType8bit(UBX_CFG_U4), 44, -1, -1},
        {"vAcc", ubxDataType8bit(UBX_CFG_U4), 48, -1, -1},
        {"velN", ubxDataType8bit(UBX_CFG_I4), 52, -1, -1},
        {"velE", ubxDataType8bit(UBX_CFG_I4), 56, -1, -1},
        {"velD", ubxDataType8bit(UBX_CFG_I4), 60, -1, -1},
        {"gSpeed", ubxDataType8bit(UBX_CFG_I4), 64, -1, -1},
        {"sAcc", ubxDataType8bit(UBX_CFG_U4), 68, -1, -1},
        {"vehRoll", ubxDataType8bit(UBX_CFG_I4), 72, -1, -1},
        {"vehPitch", ubxDataType8bit(UBX_CFG_I4), 76, -1, -1},
        {"vehHeading", ubxDataType8bit(UBX_CFG_I4), 80, -1, -1},
        {"motHeading", ubxDataType8bit(UBX_CFG_I4), 84, -1, -1},
        {"accRoll", ubxDataType8bit(UBX_CFG_U2), 88, -1, -1},
        {"accPitch", ubxDataType8bit(UBX_CFG_U2), 90, -1, -1},
        {"accHeading", ubxDataType8bit(UBX_CFG_U2), 92, -1, -1},
        {"magDec", ubxDataType8bit(UBX_CFG_I2), 94, -1, -1},
        {"magAcc", ubxDataType8bit(UBX_CFG_U2), 96, -1, -1},
        {"errEllipseOrient", ubxDataType8bit(UBX_CFG_U2), 98, -1, -1},
        {"errEllipseMajor", ubxDataType8bit(UBX_CFG_U4), 100, -1, -1},
        {"errEllipseMinor", ubxDataType8bit(UBX_CFG_U4), 104, -1, -1}};

    ubxNAVPVAT(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVPVAT);
