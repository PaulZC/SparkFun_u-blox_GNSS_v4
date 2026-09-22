/*
  ubxNAVTIMELS.h

  v4 scaffolding: UBX-NAV-TIMELS - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_TIMELS_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_TIMELS_LEN

class ubxNAVTIMELS : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_TIMELS;

    const char classStr[4] = "NAV";
    const char idStr[7] = "TIMELS";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_TIMELS_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 12;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_TIMELS_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_TIMELS_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_TIMELS_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_TIMELS_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"version", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},
        {"srcOfCurrLs", ubxDataType8bit(UBX_CFG_U1), 8, -1, -1},
        {"currLs", ubxDataType8bit(UBX_CFG_I1), 9, -1, -1},
        {"srcOfLsChange", ubxDataType8bit(UBX_CFG_U1), 10, -1, -1},
        {"lsChange", ubxDataType8bit(UBX_CFG_I1), 11, -1, -1},
        {"timeToLsEvent", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"dateOfLsGpsWn", ubxDataType8bit(UBX_CFG_U2), 16, -1, -1},
        {"dateOfLsGpsDn", ubxDataType8bit(UBX_CFG_U2), 18, -1, -1},
        {"valid", ubxDataType8bit(UBX_CFG_X1), 23, -1, -1},
        {"validCurrLs", ubxDataType8bit(UBX_CFG_L), 23, 0, 1},
        {"validTimeToLsEvent", ubxDataType8bit(UBX_CFG_L), 23, 1, 1}};

    /**
     * @brief Construct a new ubxNAVTIMELS object and register it with the UBX message registry.
     *
     * UBX-NAV-TIMELS: Leap second event information. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVTIMELS(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVTIMELS);
