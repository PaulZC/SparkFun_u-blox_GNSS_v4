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
 * @file ubxNAVPVT.h
 */

 /*
  v4 scaffolding: UBX-NAV-PVT (0x01 0x07) - Navigation position velocity time solution -
  implemented as its own Class, per AGENTS.md "Implement each UBX message in its own Class".
  This is the proof-of-concept message class for the new architecture - see AGENTS.md
  "Reference Scaffolding".

  If the user wishes to save memory, this file can be deleted - see AGENTS.md "Still undesigned
  even after this prototype" ("Conditional registration") for the work that still needs doing to
  make that fully automatic; today, deleting it also requires removing its use from
  ubxMessageVector.h's constructor.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_PVT_LEN

class ubxNAVPVT : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_PVT;

    const char classStr[4] = "NAV"; // The message Class - from the interface description
    const char idStr[4] = "PVT";    // The message ID

    const uint8_t supportedVersions = 1; // Reserved for future use with ubxMessageVersion - see AGENTS.md

    const uint16_t messageLength = UBX_NAV_PVT_LEN; // Payload length only (92 bytes) - see ubxMessage.h
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 46;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_PVT_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"year", ubxDataType8bit(UBX_CFG_U2), 4, -1, -1},
        {"month", ubxDataType8bit(UBX_CFG_U1), 6, -1, -1},
        {"day", ubxDataType8bit(UBX_CFG_U1), 7, -1, -1},
        {"hour", ubxDataType8bit(UBX_CFG_U1), 8, -1, -1},
        {"min", ubxDataType8bit(UBX_CFG_U1), 9, -1, -1},
        {"sec", ubxDataType8bit(UBX_CFG_U1), 10, -1, -1},
        {"valid", ubxDataType8bit(UBX_CFG_X1), 11, -1, -1},
        {"validDate", ubxDataType8bit(UBX_CFG_L), 11, 0, 1},
        {"validTime", ubxDataType8bit(UBX_CFG_L), 11, 1, 1},
        {"fullyResolved", ubxDataType8bit(UBX_CFG_L), 11, 2, 1},
        {"validMag", ubxDataType8bit(UBX_CFG_L), 11, 3, 1},
        {"tAcc", ubxDataType8bit(UBX_CFG_U4), 12, -1, -1},
        {"nano", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"fixType", ubxDataType8bit(UBX_CFG_U1), 20, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 21, -1, -1},
        {"gnssFixOK", ubxDataType8bit(UBX_CFG_L), 21, 0, 1},
        {"diffSoln", ubxDataType8bit(UBX_CFG_L), 21, 1, 1},
        {"psmState", ubxDataType8bit(UBX_CFG_U1), 21, 2, 3},
        {"headVehValid", ubxDataType8bit(UBX_CFG_L), 21, 5, 1},
        {"carrSoln", ubxDataType8bit(UBX_CFG_U1), 21, 6, 2},
        {"flags2", ubxDataType8bit(UBX_CFG_X1), 22, -1, -1},
        {"confirmedAvai", ubxDataType8bit(UBX_CFG_L), 22, 5, 1},
        {"confirmedDate", ubxDataType8bit(UBX_CFG_L), 22, 6, 1},
        {"confirmedTime", ubxDataType8bit(UBX_CFG_L), 22, 7, 1},
        {"numSV", ubxDataType8bit(UBX_CFG_U1), 23, -1, -1},
        {"lon", ubxDataType8bit(UBX_CFG_I4), 24, -1, -1},
        {"lat", ubxDataType8bit(UBX_CFG_I4), 28, -1, -1},
        {"height", ubxDataType8bit(UBX_CFG_I4), 32, -1, -1},
        {"hMSL", ubxDataType8bit(UBX_CFG_I4), 36, -1, -1},
        {"hAcc", ubxDataType8bit(UBX_CFG_U4), 40, -1, -1},
        {"vAcc", ubxDataType8bit(UBX_CFG_U4), 44, -1, -1},
        {"velN", ubxDataType8bit(UBX_CFG_I4), 48, -1, -1},
        {"velE", ubxDataType8bit(UBX_CFG_I4), 52, -1, -1},
        {"velD", ubxDataType8bit(UBX_CFG_I4), 56, -1, -1},
        {"gSpeed", ubxDataType8bit(UBX_CFG_I4), 60, -1, -1},
        {"headMot", ubxDataType8bit(UBX_CFG_I4), 64, -1, -1},
        {"sAcc", ubxDataType8bit(UBX_CFG_U4), 68, -1, -1},
        {"headAcc", ubxDataType8bit(UBX_CFG_U4), 72, -1, -1},
        {"pDOP", ubxDataType8bit(UBX_CFG_U2), 76, -1, -1},
        {"flags3", ubxDataType8bit(UBX_CFG_X2), 78, -1, -1},
        {"invalidLlh", ubxDataType8bit(UBX_CFG_L), 78, 0, 1},
        {"lastCorrectionAge", ubxDataType8bit(UBX_CFG_U1), 78, 1, 4},
        {"headVeh", ubxDataType8bit(UBX_CFG_I4), 84, -1, -1},
        {"magDec", ubxDataType8bit(UBX_CFG_I2), 88, -1, -1},
        {"magAcc", ubxDataType8bit(UBX_CFG_U2), 90, -1, -1}};

    /**
     * @brief Construct a new ubxNAVPVT object and register it with the UBX message registry.
     *
     * UBX-NAV-PVT: Navigation position velocity time solution. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVPVT(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys);
    }
};

ubxRegisterMessage(ubxNAVPVT);
