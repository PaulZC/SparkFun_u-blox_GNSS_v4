/*
  ubxMONHW2.h

  v4 scaffolding: UBX-MON-HW2 - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_MON_HW2_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_MON_HW2_LEN

class ubxMONHW2 : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_MON;
    const uint8_t ID = UBX_MON_HW2;

    const char classStr[4] = "MON";
    const char idStr[4] = "HW2";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_MON_HW2_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 7;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_MON_HW2_I2C, UBLOX_CFG_MSGOUT_UBX_MON_HW2_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_MON_HW2_UART1, UBLOX_CFG_MSGOUT_UBX_MON_HW2_UART2};

    const ubxField ubxFields[numFields] = {
        {"ofsI", ubxDataType8bit(UBX_CFG_I1), 0, -1, -1},
        {"magI", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},
        {"ofsQ", ubxDataType8bit(UBX_CFG_I1), 2, -1, -1},
        {"magQ", ubxDataType8bit(UBX_CFG_U1), 3, -1, -1},
        {"cfgSource", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},
        {"lowLevCfg", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"postStatus", ubxDataType8bit(UBX_CFG_U4), 20, -1, -1}};

    ubxMONHW2(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxMONHW2);
