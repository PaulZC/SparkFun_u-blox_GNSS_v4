/**
 * @date 2026
 * @copyright Copyright (c) 2026, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 * 
 * Please see LICENSE.md for more details
 * 
 * An Arduino Library which allows you to communicate seamlessly with u-blox GNSS modules using the Configuration Interface
 * 
 * SparkFun sells these at its website: www.sparkfun.com
 * Do you like this library? Help support SparkFun. Buy a board!
 * https://www.sparkfun.com/sparkfun-allband-gnss-rtk-breakout-zed-x20p-qwiic.html
 * https://www.sparkfun.com/sparkfun-gps-rtk2-board-zed-f9p-qwiic-gps-15136.html
 * https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html
 * https://www.sparkfun.com/sparkfun-gnss-receiver-breakout-max-m10s-qwiic.html
 * https://www.sparkfun.com/sparkfun-gps-rtk-dead-reckoning-breakout-zed-f9r-qwiic-gps-22693.html
 *
 * Original version by Nathan Seidle @ SparkFun Electronics, September 6th, 2018
 * v2.0 rework by Paul Clark @ SparkFun Electronics, December 31st, 2020
 * v3.0 rework by Paul Clark @ SparkFun Electronics, December 8th, 2022
 * v4.0 rework by Claude, directed by Paul Clark @ SparkFun Electronics, September 2026
 *
 * https://github.com/sparkfun/SparkFun_u-blox_GNSS_v4
 *
 * 
 * @file ubxMONHW.h
 *
 * v4 scaffolding: UBX-MON-HW - implemented as its own Class.
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

    /**
     * @brief Construct a new ubxMONHW object and register it with the UBX message registry.
     *
     * UBX-MON-HW: Hardware status. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxMONHW(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxMONHW);
