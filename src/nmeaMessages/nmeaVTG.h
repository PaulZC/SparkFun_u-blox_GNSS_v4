/*
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
 * @file nmeaVTG.h
 *
 * Definitions for the National Marine Electronics Association standard NMEA 0183
 * VTG message: Course over ground and Ground speed
 *
 * The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
 * https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf
 *
 */

#pragma once

#include "../nmeaMessage.h"

class nmeaVTG : public nmeaMessage
{
public:
    const char msgId[4] = "VTG"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters
    const uint8_t messageLength = NMEA_VTG_MAX_LENGTH;

    static const uint8_t numFields = 10;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_VTG_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_VTG_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_VTG_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_VTG_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxVTG", nmeaDataTypeString, 0},
        {"cogt", nmeaDataTypeNumeric, 1},
        {"cogtUnit", nmeaDataTypeChar, 2},
        {"cogm", nmeaDataTypeNumeric, 3},
        {"cogmUnit", nmeaDataTypeChar, 4},
        {"sogn", nmeaDataTypeNumeric, 5},
        {"sognUnit", nmeaDataTypeChar, 6},
        {"sogk", nmeaDataTypeNumeric, 7},
        {"sogkUnit", nmeaDataTypeChar, 8},
        {"posMode", nmeaDataTypeChar, 9},
    };

    /**
     * @brief Construct a new nmeaVTG object and register it with the NMEA message registry.
     *
     * NMEA VTG: Course over ground and Ground speed. Registers this message's identifier, field table, callback-copy count and
     * per-port output-enable keys with the base nmeaMessage class via addNMEA(), making it
     * discoverable by the generic NMEA dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    nmeaVTG(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaVTG);
