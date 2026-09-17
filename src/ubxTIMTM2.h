/*
  ubxTIMTM2.h

  v4 scaffolding: UBX-TIM-TM2 - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_TIM_TM2_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "ubxMessage.h"
#include "u-blox_config_keys.h"
#include "u-blox_Class_and_ID.h"
#include "u-blox_structs.h" // For UBX_TIM_TM2_LEN

class ubxTIMTM2 : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_TIM;
    const uint8_t ID = UBX_TIM_TM2;

    const char classStr[4] = "TIM";
    const char idStr[4] = "TM2";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_TIM_TM2_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 17;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_TIM_TM2_I2C, UBLOX_CFG_MSGOUT_UBX_TIM_TM2_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_TIM_TM2_UART1, UBLOX_CFG_MSGOUT_UBX_TIM_TM2_UART2};

    const ubxField ubxFields[numFields] = {
        {"ch", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 1, -1, -1},
        {"mode", ubxDataType8bit(UBX_CFG_L), 1, 0, 1},
        {"run", ubxDataType8bit(UBX_CFG_L), 1, 1, 1},
        {"newFallingEdge", ubxDataType8bit(UBX_CFG_L), 1, 2, 1},
        {"timeBase", ubxDataType8bit(UBX_CFG_U1), 1, 3, 2},
        {"utc", ubxDataType8bit(UBX_CFG_L), 1, 5, 1},
        {"time", ubxDataType8bit(UBX_CFG_L), 1, 6, 1},
        {"newRisingEdge", ubxDataType8bit(UBX_CFG_L), 1, 7, 1},
        {"count", ubxDataType8bit(UBX_CFG_U2), 2, -1, -1},
        {"wnR", ubxDataType8bit(UBX_CFG_U2), 4, -1, -1},
        {"wnF", ubxDataType8bit(UBX_CFG_U2), 6, -1, -1},
        {"towMsR", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"towSubMsR", ubxDataType8bit(UBX_CFG_U4), 12, -1, -1},
        {"towMsF", ubxDataType8bit(UBX_CFG_U4), 16, -1, -1},
        {"towSubMsF", ubxDataType8bit(UBX_CFG_U4), 20, -1, -1},
        {"accEst", ubxDataType8bit(UBX_CFG_U4), 24, -1, -1}};

    ubxTIMTM2(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxTIMTM2);
