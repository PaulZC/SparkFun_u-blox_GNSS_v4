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
 * @file nmeaGGA.h
 *
 * Definitions for the National Marine Electronics Association standard NMEA 0183
 * GGA message: Global Positioning System Fix Data
 *
 * The nmeaField.fieldName is taken from the u-blox Interface Description, e.g.:
 * https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf
 *
 */

#pragma once

#include "../nmeaMessage.h"

class nmeaGGA : public nmeaMessage
{
public:
    const char msgId[4] = "GGA"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    const uint8_t numCallbackCopies = 1;

    // NMEA defines a maximum length of 79 characters, but GNSS output can include additional decimal places
    const uint8_t messageLength = NMEA_GGA_MAX_LENGTH;

    static const uint8_t numFields = 15;

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_GGA_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_GGA_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_GGA_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_GGA_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxGGA", nmeaDataTypeString, 0},
        {"time", nmeaDataTypeTime, 1},
        {"lat", nmeaDataTypeDDMM, 2},
        {"NS", nmeaDataTypeChar, 3},
        {"lon", nmeaDataTypeDDDMM, 4},
        {"EW", nmeaDataTypeChar, 5},
        {"quality", nmeaDataTypeDigit, 6},
        {"numSV", nmeaDataTypeNumeric, 7},
        {"HDOP", nmeaDataTypeNumeric, 8},
        {"alt", nmeaDataTypeNumeric, 9},
        {"altUnit", nmeaDataTypeChar, 10},
        {"sep", nmeaDataTypeNumeric, 11},
        {"sepUnit", nmeaDataTypeChar, 12},
        {"diffAge", nmeaDataTypeNumeric, 13},
        {"diffStation", nmeaDataTypeNumeric, 14},
};

    /**
     * @brief Construct a new nmeaGGA object and register it with the NMEA message registry.
     *
     * NMEA GGA: Global Positioning System Fix Data. Registers this message's identifier, field table, callback-copy count and
     * per-port output-enable keys with the base nmeaMessage class via addNMEA(), making it
     * discoverable by the generic NMEA dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    nmeaGGA(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys);
    }
};

nmeaRegisterMessage(nmeaGGA);
