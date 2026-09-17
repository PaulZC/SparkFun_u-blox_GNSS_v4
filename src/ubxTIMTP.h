/*
  ubxTIMTP.h

  v4 scaffolding: UBX-TIM-TP - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_TIM_TP_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "ubxMessage.h"
#include "u-blox_config_keys.h"
#include "u-blox_Class_and_ID.h"
#include "u-blox_structs.h" // For UBX_TIM_TP_LEN

class ubxTIMTP : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_TIM;
    const uint8_t ID = UBX_TIM_TP;

    const char classStr[4] = "TIM";
    const char idStr[3] = "TP";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_TIM_TP_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 12;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_TIM_TP_I2C, UBLOX_CFG_MSGOUT_UBX_TIM_TP_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_TIM_TP_UART1, UBLOX_CFG_MSGOUT_UBX_TIM_TP_UART2};

    const ubxField ubxFields[numFields] = {
        {"towMS", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"towSubMS", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"qErr", ubxDataType8bit(UBX_CFG_I4), 8, -1, -1},
        {"week", ubxDataType8bit(UBX_CFG_U2), 12, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 14, -1, -1},
        {"timeBase", ubxDataType8bit(UBX_CFG_L), 14, 0, 1},
        {"utc", ubxDataType8bit(UBX_CFG_L), 14, 1, 1},
        {"raim", ubxDataType8bit(UBX_CFG_U1), 14, 2, 2},
        {"qErrInvalid", ubxDataType8bit(UBX_CFG_L), 14, 4, 1},
        {"refInfo", ubxDataType8bit(UBX_CFG_X1), 15, -1, -1},
        {"timeRefGnss", ubxDataType8bit(UBX_CFG_U1), 15, 0, 4},
        {"utcStandard", ubxDataType8bit(UBX_CFG_U1), 15, 4, 4}};

    ubxTIMTP(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxTIMTP);
