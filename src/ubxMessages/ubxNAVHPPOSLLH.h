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
 * @file ubxNAVHPPOSLLH.h
 *
 * v4 scaffolding: UBX-NAV-HPPOSLLH - implemented as its own Class.
 */

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_HPPOSLLH_LEN

class ubxNAVHPPOSLLH : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_HPPOSLLH;

    const char classStr[4] = "NAV";
    const char idStr[9] = "HPPOSLLH";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_HPPOSLLH_LEN;
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 14;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 3, -1, -1},
        {"invalidLlh", ubxDataType8bit(UBX_CFG_L), 3, 0, 1},
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"lon", ubxDataType8bit(UBX_CFG_I4), 8, -1, -1},
        {"lat", ubxDataType8bit(UBX_CFG_I4), 12, -1, -1},
        {"height", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"hMSL", ubxDataType8bit(UBX_CFG_I4), 20, -1, -1},
        {"lonHp", ubxDataType8bit(UBX_CFG_I1), 24, -1, -1},
        {"latHp", ubxDataType8bit(UBX_CFG_I1), 25, -1, -1},
        {"heightHp", ubxDataType8bit(UBX_CFG_I1), 26, -1, -1},
        {"hMSLHp", ubxDataType8bit(UBX_CFG_I1), 27, -1, -1},
        {"hAcc", ubxDataType8bit(UBX_CFG_U4), 28, -1, -1},
        {"vAcc", ubxDataType8bit(UBX_CFG_U4), 32, -1, -1}};

    /**
     * @brief Construct a new ubxNAVHPPOSLLH object and register it with the UBX message registry.
     *
     * UBX-NAV-HPPOSLLH: High precision geodetic position solution. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVHPPOSLLH(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVHPPOSLLH);
