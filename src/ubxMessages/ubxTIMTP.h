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
 * @file ubxTIMTP.h
 */

 /*
  v4 scaffolding: UBX-TIM-TP - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Auto-generated field table from u-blox_structs.h's
  UBX_TIM_TP_data_t (verified against the byte offsets used by the existing v3 extract*() calls in
  processUBXpacket()). See AGENTS.md "Reference Scaffolding".
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_TIM_TP_LEN

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

    /**
     * @brief Construct a new ubxTIMTP object and register it with the UBX message registry.
     *
     * UBX-TIM-TP: Time pulse time data. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxTIMTP(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxTIMTP);
