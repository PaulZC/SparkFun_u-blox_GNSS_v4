/*
  ubxHNRPVT.h

  v4 scaffolding: UBX-HNR-PVT - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_HNR_PVT_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_HNR_PVT_LEN

class ubxHNRPVT : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_HNR;
    const uint8_t ID = UBX_HNR_PVT;

    const char classStr[4] = "HNR";
    const char idStr[4] = "PVT";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_HNR_PVT_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 31;
    // No CFG-MSGOUT-* keys exist for this message - its rate is controlled via a different
    // mechanism (see AGENTS.md). msgOutKeys stays all-zero; getMsgOutKey() will return 0.
    const uint32_t msgOutKeys[4] = {0, 0, 0, 0};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"year", ubxDataType8bit(UBX_CFG_U2), 4, -1, -1},
        {"month", ubxDataType8bit(UBX_CFG_U1), 6, -1, -1},
        {"day", ubxDataType8bit(UBX_CFG_U1), 7, -1, -1},
        {"hour", ubxDataType8bit(UBX_CFG_U1), 8, -1, -1},
        {"min", ubxDataType8bit(UBX_CFG_U1), 9, -1, -1},
        {"sec", ubxDataType8bit(UBX_CFG_U1), 10, -1, -1},
        {"valid", ubxDataType8bit(UBX_CFG_X1), 11, -1, -1},
        {"validDate", ubxDataType8bit(UBX_CFG_L), 11, 0, 1},
        {"validTime", ubxDataType8bit(UBX_CFG_L), 11, 1, 1},
        {"fullyResolved", ubxDataType8bit(UBX_CFG_L), 11, 2, 1},
        {"nano", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"gpsFix", ubxDataType8bit(UBX_CFG_U1), 16, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 17, -1, -1},
        {"gpsFixOK", ubxDataType8bit(UBX_CFG_L), 17, 0, 1},
        {"diffSoln", ubxDataType8bit(UBX_CFG_L), 17, 1, 1},
        {"WKNSET", ubxDataType8bit(UBX_CFG_L), 17, 2, 1},
        {"TOWSET", ubxDataType8bit(UBX_CFG_L), 17, 3, 1},
        {"headVehValid", ubxDataType8bit(UBX_CFG_L), 17, 4, 1},
        {"lon", ubxDataType8bit(UBX_CFG_I4), 20, -1, -1},
        {"lat", ubxDataType8bit(UBX_CFG_I4), 24, -1, -1},
        {"height", ubxDataType8bit(UBX_CFG_I4), 28, -1, -1},
        {"hMSL", ubxDataType8bit(UBX_CFG_I4), 32, -1, -1},
        {"gSpeed", ubxDataType8bit(UBX_CFG_I4), 36, -1, -1},
        {"speed", ubxDataType8bit(UBX_CFG_I4), 40, -1, -1},
        {"headMot", ubxDataType8bit(UBX_CFG_I4), 44, -1, -1},
        {"headVeh", ubxDataType8bit(UBX_CFG_I4), 48, -1, -1},
        {"hAcc", ubxDataType8bit(UBX_CFG_U4), 52, -1, -1},
        {"vAcc", ubxDataType8bit(UBX_CFG_U4), 56, -1, -1},
        {"sAcc", ubxDataType8bit(UBX_CFG_U4), 60, -1, -1},
        {"headAcc", ubxDataType8bit(UBX_CFG_U4), 64, -1, -1}};

    /**
     * @brief Construct a new ubxHNRPVT object and register it with the UBX message registry.
     *
     * UBX-HNR-PVT: High rate output of PVT solution. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxHNRPVT(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxHNRPVT);
