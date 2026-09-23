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
 * @file nmeaGSV.h
 *
 * Definitions for the National Marine Electronics Association standard NMEA 0183
 * GSV message: GNSS Satellites in View
 *
 * Unlike every other supported NMEA message, GSV is variable length: a header (xxGSV, numMsg,
 * msgNum, numSV) is followed by 1 to 4 repeated blocks (svid, elv, az, cno - one per satellite),
 * followed by a footer (signalId). Up to 9 GSV sentences can be sent per constellation per
 * navigation cycle. See AGENTS.md "Adding support for NMEA GSV messages" for the design this is
 * built on (nmeaMessage's _blockFields/_numHeaderFields/_maxNumBlocks and its ring-buffered
 * _callbackStorage).
 *
 * The nmeaField.fieldName is taken from the u-blox Interface Description:
 * u-blox-X20-HPG-2.10_InterfaceDescription_UBXDOC-304424225-21263.pdf
 *
 */

#pragma once

#include "../nmeaMessage.h"

class nmeaGSV : public nmeaMessage
{
public:
    const char msgId[4] = "GSV"; // The message identifier

    const uint8_t supportedVersions = 1; // Reserved for future use

    // Up to 9 GSV messages can arrive per constellation (GP/GL/GA/GB/GI/GQ) within one
    // navigation cycle - 6 constellations * 9 = 54 - so the callback ring buffer needs 54 slots
    // to avoid dropping any of them before checkCallbacks() drains it. See AGENTS.md "Adding
    // support for NMEA GSV messages".
    const uint8_t numCallbackCopies = 54;

    // NMEA defines a maximum length of 79 characters. A full 4-satellite GSV sentence
    // ("$GPGSV,9,9,99,99,90,359,99,99,90,359,99,99,90,359,99,99,90,359,99,1*hh") is ~80
    // characters - 100 keeps the same headroom convention as nmeaGNS.h/nmeaGST.h/nmeaRMC.h.
    const uint8_t messageLength = NMEA_GSV_MAX_LENGTH;

    // Header: xxGSV, numMsg, msgNum, numSV. Footer: signalId. numFields is interpreted as
    // excluding the repeated blocks - see nmeaMessage::extractFieldFrom() and AGENTS.md.
    static const uint8_t numHeaderFields = 4;
    static const uint8_t numFields = 5; // numHeaderFields (4) + footer (1)

    // One block per satellite: svid, elv, az, cno.
    static const uint8_t numBlockFields = 4;
    static const uint8_t maxNumBlocks = 4; // Up to 4 satellites per GSV sentence

    // The configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_GSV_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_GSV_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_GSV_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_GSV_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxGSV", nmeaDataTypeString, 0}, // Header (implied by fieldNumber < numHeaderFields)
        {"numMsg", nmeaDataTypeDigit, 1}, // Header (implied by fieldNumber < numHeaderFields)
        {"msgNum", nmeaDataTypeDigit, 2}, // Header (implied by fieldNumber < numHeaderFields)
        {"numSV", nmeaDataTypeNumeric, 3}, // Header (implied by fieldNumber < numHeaderFields)
        {"signalId", nmeaDataTypeString, 4}, // Footer (implied by fieldNumber >= numHeaderFields). Hexadecimal - kept as String, not Numeric
    };

    const nmeaField nmeaBlockFields[numBlockFields] = {
        {"svid", nmeaDataTypeNumeric, 0},
        {"elv", nmeaDataTypeNumeric, 1},
        {"az", nmeaDataTypeNumeric, 2},
        {"cno", nmeaDataTypeNumeric, 3},
    };

    /**
     * @brief Construct a new nmeaGSV object and register it with the NMEA message registry.
     *
     * NMEA GSV: GNSS Satellites in View. Registers this message's identifier, field table, callback-copy count and
     * per-port output-enable keys with the base nmeaMessage class via addNMEA(), making it
     * discoverable by the generic NMEA dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    nmeaGSV(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys,
                (const void *)&nmeaBlockFields, numBlockFields, numHeaderFields, maxNumBlocks);
    }
};

nmeaRegisterMessage(nmeaGSV);
