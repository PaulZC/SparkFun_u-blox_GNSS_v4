/*
  ubxMONHW.h

  v4 scaffolding: UBX-MON-HW - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_MON_HW_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_MON_HW_LEN

class ubxMONHW : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_MON;
    const uint8_t ID = UBX_MON_HW;

    const char classStr[4] = "MON";
    const char idStr[3] = "HW";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_MON_HW_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 18;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_MON_HW_I2C, UBLOX_CFG_MSGOUT_UBX_MON_HW_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_MON_HW_UART1, UBLOX_CFG_MSGOUT_UBX_MON_HW_UART2};

    const ubxField ubxFields[numFields] = {
        {"pinSel", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"pinBank", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"pinDir", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"pinVal", ubxDataType8bit(UBX_CFG_U4), 12, -1, -1},
        {"noisePerMS", ubxDataType8bit(UBX_CFG_U2), 16, -1, -1},
        {"agcCnt", ubxDataType8bit(UBX_CFG_U2), 18, -1, -1},
        {"aStatus", ubxDataType8bit(UBX_CFG_U1), 20, -1, -1},
        {"aPower", ubxDataType8bit(UBX_CFG_U1), 21, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 22, -1, -1},
        {"rtcCalib", ubxDataType8bit(UBX_CFG_L), 22, 0, 1},
        {"safeBoot", ubxDataType8bit(UBX_CFG_L), 22, 1, 1},
        {"jammingState", ubxDataType8bit(UBX_CFG_U1), 22, 2, 2},
        {"xtalAbsent", ubxDataType8bit(UBX_CFG_L), 22, 4, 1},
        {"usedMask", ubxDataType8bit(UBX_CFG_U4), 24, -1, -1},
        {"jamInd", ubxDataType8bit(UBX_CFG_U1), 45, -1, -1},
        {"pinIrq", ubxDataType8bit(UBX_CFG_U4), 48, -1, -1},
        {"pullH", ubxDataType8bit(UBX_CFG_U4), 52, -1, -1},
        {"pullL", ubxDataType8bit(UBX_CFG_U4), 56, -1, -1}};

    ubxMONHW(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxMONHW);
