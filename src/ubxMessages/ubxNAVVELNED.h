/*
  ubxNAVVELNED.h

  v4 scaffolding: UBX-NAV-VELNED - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_VELNED_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_VELNED_LEN

class ubxNAVVELNED : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_VELNED;

    const char classStr[4] = "NAV";
    const char idStr[7] = "VELNED";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_VELNED_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 9;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_VELNED_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_VELNED_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_VELNED_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_VELNED_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"velN", ubxDataType8bit(UBX_CFG_I4), 4, -1, -1},
        {"velE", ubxDataType8bit(UBX_CFG_I4), 8, -1, -1},
        {"velD", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"speed", ubxDataType8bit(UBX_CFG_U4), 16, -1, -1},
        {"gSpeed", ubxDataType8bit(UBX_CFG_U4), 20, -1, -1},
        {"heading", ubxDataType8bit(UBX_CFG_I4), 24, -1, -1},
        {"sAcc", ubxDataType8bit(UBX_CFG_U4), 28, -1, -1},
        {"cAcc", ubxDataType8bit(UBX_CFG_U4), 32, -1, -1}};

    /**
     * @brief Construct a new ubxNAVVELNED object and register it with the UBX message registry.
     *
     * UBX-NAV-VELNED: Velocity solution in NED. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVVELNED(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVVELNED);
