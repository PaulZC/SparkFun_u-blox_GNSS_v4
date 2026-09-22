/*
  ubxNAVSTATUS.h

  v4 scaffolding: UBX-NAV-STATUS - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_STATUS_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_STATUS_LEN

class ubxNAVSTATUS : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_STATUS;

    const char classStr[4] = "NAV";
    const char idStr[7] = "STATUS";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_STATUS_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 17;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"gpsFix", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 5, -1, -1},
        {"gpsFixOk", ubxDataType8bit(UBX_CFG_L), 5, 0, 1},
        {"diffSoln", ubxDataType8bit(UBX_CFG_L), 5, 1, 1},
        {"wknSet", ubxDataType8bit(UBX_CFG_L), 5, 2, 1},
        {"towSet", ubxDataType8bit(UBX_CFG_L), 5, 3, 1},
        {"fixStat", ubxDataType8bit(UBX_CFG_X1), 6, -1, -1},
        {"diffCorr", ubxDataType8bit(UBX_CFG_L), 6, 0, 1},
        {"carrSolnValid", ubxDataType8bit(UBX_CFG_L), 6, 1, 1},
        {"mapMatching", ubxDataType8bit(UBX_CFG_U1), 6, 6, 2},
        {"flags2", ubxDataType8bit(UBX_CFG_X1), 7, -1, -1},
        {"psmState", ubxDataType8bit(UBX_CFG_U1), 7, 0, 2},
        {"spoofDetState", ubxDataType8bit(UBX_CFG_U1), 7, 3, 2},
        {"carrSoln", ubxDataType8bit(UBX_CFG_U1), 7, 6, 2},
        {"ttff", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"msss", ubxDataType8bit(UBX_CFG_U4), 12, -1, -1}};

    /**
     * @brief Construct a new ubxNAVSTATUS object and register it with the UBX message registry.
     *
     * UBX-NAV-STATUS: Receiver navigation status. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVSTATUS(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVSTATUS);
