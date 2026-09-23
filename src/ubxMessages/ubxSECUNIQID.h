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
 * @file ubxSECUNIQID.h
 */

/*
  v4 scaffolding: UBX-SEC-UNIQID - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". The payload is a fixed 4-byte header (version,
  reserved0[3]) followed by the chip's unique ID: 5 bytes on the F9/M9 (version 1, total payload
  length UBX_SEC_UNIQID_LEN_VERSION1 = 9) or 6 bytes on the M10 (version 2, total payload length
  UBX_SEC_UNIQID_LEN_VERSION2 = 10) - see u-blox_structs.h's (removed) UBX_SEC_UNIQID_data_t.

  Rather than a fixed 6-byte array field (which would read one stale/uninitialized byte on a
  version-1 module that only ever sends 5), uniqueId is modelled the same way RXM-PMP's userData/
  RXM-QZSSL6's msgBytes are: as 0..6 repeated 1-byte "blocks", one field ("byte") each. This is
  also exactly ESF-RAW's case (see AGENTS.md "Adding support for ESF-RAW and ESF-STATUS"): there
  is no header field carrying the byte count, so getUbxMessageBlockCount()/...Callback() fall back
  to deriving it purely from the actual received length (5 for version 1, 6 for version 2) -
  blockCountField is left at its default nullptr, same as ESF-RAW.

  numCallbackCopies is 1 - like MON-HW/MON-HW2/MON-RF, this is a poll-only status message, not a
  message that arrives in bursts.

  There are no CFG-MSGOUT-UBX-SEC-UNIQID-* keys - the chip ID never changes, so u-blox gives this
  message no periodic-output configuration at all; it can only be polled. msgOutKeys stays
  all-zero, same convention as HNR-ATT/HNR-INS/HNR-PVT (see AGENTS.md).

  Replaces the old v3-style getUniqueChipId(UBX_SEC_UNIQID_data_t *data, ...)/
  getUniqueChipIdStr(UBX_SEC_UNIQID_data_t *data, ...) (which polled with packetCfg/sendCommand()
  and hand-extracted every byte, returning a static char buffer) with the usual getSECUNIQID()
  thin wrapper around getUBX(), plus a getUniqueChipIdStr() helper that reads the 6 "byte" blocks
  and returns them as a 12-character hex String (e.g. "0123456789AB"). See u-blox_GNSS.h/.cpp.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_SEC_UNIQID_LEN_VERSION2

class ubxSECUNIQID : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_SEC;
    const uint8_t ID = UBX_SEC_UNIQID;

    const char classStr[4] = "SEC";
    const char idStr[7] = "UNIQID";

    const uint8_t supportedVersions = 1; // Both version 1 (5-byte ID) and version 2 (6-byte ID)
    // are handled by this SAME class/layout (see the block-count-by-actual-length note above) -
    // there is no version-dependent field-placement difference to pick one of, unlike SEC-SIG.

    const uint16_t messageLength = UBX_SEC_UNIQID_LEN_VERSION2; // Maximum payload length (version 2 / M10) - see above
    const uint8_t numCallbackCopies = 1;

    // Header field table - the 4 bytes that precede the uniqueId bytes. Read with the ordinary
    // getUbxMessageField() / getUbxMessageFieldCallback(), exactly like any other message.
    static const uint8_t numFields = 1;
    // No CFG-MSGOUT-* keys exist for this message - it can only be polled - see above.
    const uint32_t msgOutKeys[4] = {0, 0, 0, 0};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1}};
        // reserved0[3] at bytes 1-3 is not exposed

    // Block field table - uniqueId modelled as 0..6 repeated single-byte "blocks" - see above.
    // Read with getUbxMessageBlockField() / getUbxMessageBlockFieldCallback(), bounded by
    // getUbxMessageBlockCount() / ...Callback() (there is no header field to read the count from
    // directly - see above).
    static const uint8_t numBlockFields = 1;
    static const uint16_t blockHeaderLength = 4; // Bytes before the first uniqueId byte
    static const uint16_t blockLength = 1;       // Bytes per "block" (one uniqueId byte)

    const ubxField ubxBlockFields[numBlockFields] = {
        {"byte", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1}};

    /**
     * @brief Construct a new ubxSECUNIQID object and register it with the UBX message registry.
     *
     * UBX-SEC-UNIQID: Unique chip ID. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxSECUNIQID(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   6); // maxBlocks: at most 6 uniqueId bytes (M10 / version 2)
    }
};

ubxRegisterMessage(ubxSECUNIQID);
