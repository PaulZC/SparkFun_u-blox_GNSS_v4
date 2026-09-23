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
 * @file ubxRXMCOR.h
 *
 * v4 scaffolding: UBX-RXM-COR - implemented as its own Class.
 */

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_RXM_COR_LEN

class ubxRXMCOR : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_RXM;
    const uint8_t ID = UBX_RXM_COR;

    const char classStr[4] = "RXM";
    const char idStr[4] = "COR";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_RXM_COR_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 14;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_RXM_COR_I2C, UBLOX_CFG_MSGOUT_UBX_RXM_COR_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_RXM_COR_UART1, UBLOX_CFG_MSGOUT_UBX_RXM_COR_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"ebno", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},
        {"statusInfo", ubxDataType8bit(UBX_CFG_X4), 4, -1, -1},
        {"protocol", ubxDataType8bit(UBX_CFG_U1), 4, 0, 5},
        {"errStatus", ubxDataType8bit(UBX_CFG_U1), 4, 5, 2},
        {"msgUsed", ubxDataType8bit(UBX_CFG_U1), 4, 7, 2},
        {"correctionId", ubxDataType8bit(UBX_CFG_U1), 4, 9, 16},
        {"msgTypeValid", ubxDataType8bit(UBX_CFG_L), 4, 25, 1},
        {"msgSubTypeValid", ubxDataType8bit(UBX_CFG_L), 4, 26, 1},
        {"msgInputHandle", ubxDataType8bit(UBX_CFG_L), 4, 27, 1},
        {"msgEncrypted", ubxDataType8bit(UBX_CFG_U1), 4, 28, 2},
        {"msgDecrypted", ubxDataType8bit(UBX_CFG_U1), 4, 30, 2},
        {"msgType", ubxDataType8bit(UBX_CFG_U2), 8, -1, -1},
        {"msgSubType", ubxDataType8bit(UBX_CFG_U2), 10, -1, -1}};

    /**
     * @brief Construct a new ubxRXMCOR object and register it with the UBX message registry.
     *
     * UBX-RXM-COR: Differential correction input status. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxRXMCOR(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxRXMCOR);
