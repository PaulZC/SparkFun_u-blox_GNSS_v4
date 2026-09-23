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
 * @file ubxNAVSVIN.h
 *
 * v4 scaffolding: UBX-NAV-SVIN - implemented as its own Class.
 */

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_SVIN_LEN

class ubxNAVSVIN : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_SVIN;

    const char classStr[4] = "NAV";
    const char idStr[5] = "SVIN";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_SVIN_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 13;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_SVIN_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_SVIN_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_SVIN_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_SVIN_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"dur", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"meanX", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"meanY", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"meanZ", ubxDataType8bit(UBX_CFG_I4), 20, -1, -1},
        {"meanXHP", ubxDataType8bit(UBX_CFG_I1), 24, -1, -1},
        {"meanYHP", ubxDataType8bit(UBX_CFG_I1), 25, -1, -1},
        {"meanZHP", ubxDataType8bit(UBX_CFG_I1), 26, -1, -1},
        {"meanAcc", ubxDataType8bit(UBX_CFG_U4), 28, -1, -1},
        {"obs", ubxDataType8bit(UBX_CFG_U4), 32, -1, -1},
        {"valid", ubxDataType8bit(UBX_CFG_I1), 36, -1, -1},
        {"active", ubxDataType8bit(UBX_CFG_I1), 37, -1, -1}};

    /**
     * @brief Construct a new ubxNAVSVIN object and register it with the UBX message registry.
     *
     * UBX-NAV-SVIN: Survey-in data. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVSVIN(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVSVIN);
