/*
  ubxNAVHPPOSLLH.h

  v4 scaffolding: UBX-NAV-HPPOSLLH - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_HPPOSLLH_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_HPPOSLLH_LEN

class ubxNAVHPPOSLLH : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_HPPOSLLH;

    const char classStr[4] = "NAV";
    const char idStr[9] = "HPPOSLLH";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_HPPOSLLH_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 14;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 3, -1, -1},
        {"invalidLlh", ubxDataType8bit(UBX_CFG_L), 3, 0, 1},
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"lon", ubxDataType8bit(UBX_CFG_I4), 8, -1, -1},
        {"lat", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"height", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"hMSL", ubxDataType8bit(UBX_CFG_I4), 20, -1, -1},
        {"lonHp", ubxDataType8bit(UBX_CFG_I1), 24, -1, -1},
        {"latHp", ubxDataType8bit(UBX_CFG_I1), 25, -1, -1},
        {"heightHp", ubxDataType8bit(UBX_CFG_I1), 26, -1, -1},
        {"hMSLHp", ubxDataType8bit(UBX_CFG_I1), 27, -1, -1},
        {"hAcc", ubxDataType8bit(UBX_CFG_U4), 28, -1, -1},
        {"vAcc", ubxDataType8bit(UBX_CFG_U4), 32, -1, -1}};

    /**
     * @brief Construct a new ubxNAVHPPOSLLH object and register it with the UBX message registry.
     *
     * UBX-NAV-HPPOSLLH: High precision geodetic position solution. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVHPPOSLLH(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVHPPOSLLH);
