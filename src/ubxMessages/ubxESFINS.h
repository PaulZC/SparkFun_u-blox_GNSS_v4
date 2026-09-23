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
 * @file ubxESFINS.h
 *
 * v4 scaffolding: UBX-ESF-INS - implemented as its own Class.
 */

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_ESF_INS_LEN

class ubxESFINS : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_ESF;
    const uint8_t ID = UBX_ESF_INS;

    const char classStr[4] = "ESF";
    const char idStr[4] = "INS";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_ESF_INS_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 15;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_ESF_INS_I2C, UBLOX_CFG_MSGOUT_UBX_ESF_INS_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_ESF_INS_UART1, UBLOX_CFG_MSGOUT_UBX_ESF_INS_UART2};

    const ubxField ubxFields[numFields] = {
        {"bitfield0", ubxDataType8bit(UBX_CFG_X4), 0, -1, -1},
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, 0, 8},
        {"xAngRateValid", ubxDataType8bit(UBX_CFG_L), 0, 8, 1},
        {"yAngRateValid", ubxDataType8bit(UBX_CFG_L), 0, 9, 1},
        {"zAngRateValid", ubxDataType8bit(UBX_CFG_L), 0, 10, 1},
        {"xAccelValid", ubxDataType8bit(UBX_CFG_L), 0, 11, 1},
        {"yAccelValid", ubxDataType8bit(UBX_CFG_L), 0, 12, 1},
        {"zAccelValid", ubxDataType8bit(UBX_CFG_L), 0, 13, 1},
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"xAngRate", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"yAngRate", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"zAngRate", ubxDataType8bit(UBX_CFG_I4), 20, -1, -1},
        {"xAccel", ubxDataType8bit(UBX_CFG_I4), 24, -1, -1},
        {"yAccel", ubxDataType8bit(UBX_CFG_I4), 28, -1, -1},
        {"zAccel", ubxDataType8bit(UBX_CFG_I4), 32, -1, -1}};

    /**
     * @brief Construct a new ubxESFINS object and register it with the UBX message registry.
     *
     * UBX-ESF-INS: Vehicle dynamics information. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxESFINS(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxESFINS);
