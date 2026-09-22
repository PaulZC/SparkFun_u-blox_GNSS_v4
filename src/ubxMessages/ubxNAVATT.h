/*
  ubxNAVATT.h

  v4 scaffolding: UBX-NAV-ATT - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_NAV_ATT_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_ATT_LEN

class ubxNAVATT : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_ATT;

    const char classStr[4] = "NAV";
    const char idStr[4] = "ATT";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_ATT_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 8;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_ATT_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_ATT_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_ATT_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_ATT_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"version", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},
        {"roll", ubxDataType8bit(UBX_CFG_I4), 8, -1, -1},
        {"pitch", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"heading", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"accRoll", ubxDataType8bit(UBX_CFG_U4), 20, -1, -1},
        {"accPitch", ubxDataType8bit(UBX_CFG_U4), 24, -1, -1},
        {"accHeading", ubxDataType8bit(UBX_CFG_U4), 28, -1, -1}};

    /**
     * @brief Construct a new ubxNAVATT object and register it with the UBX message registry.
     *
     * UBX-NAV-ATT: Vehicle attitude solution. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVATT(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVATT);
