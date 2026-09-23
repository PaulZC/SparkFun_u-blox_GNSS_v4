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
 * @file ubxNAVSIG.h
 */

/*
  v4 scaffolding: UBX-NAV-SIG - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Like NAV-SAT (see ubxNAVSAT.h), NAV-SIG's
  payload is variable length: a fixed 8-byte header (iTOW, version, numSigs) followed by
  0..UBX_NAV_SIG_MAX_BLOCKS identically-shaped 16-byte per-signal blocks - see u-blox_structs.h's
  UBX_NAV_SIG_header_t/UBX_NAV_SIG_block_t and AGENTS.md "Adding the variable-length UBX
  messages". This is the second (and, for now, last) message migrated using that mechanism -
  NAV-SAT was the worked example; nothing new needed designing here, the same approach applies
  directly.

  'ubxFields' below describes the header only, read with the ordinary
  getUbxMessageField()/getUbxMessageFieldCallback(). 'ubxBlockFields' describes one repeated
  block, with offsets relative to the START OF THE BLOCK (not the message) - read with
  getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing the block index
  (0..numSigs-1) returned by the "numSigs" header field above.

  messageLength is set to UBX_NAV_SIG_MAX_LEN - the MAXIMUM possible payload, not a fixed wire
  size - exactly as for NAV-SAT. storePayload() (in ubxMessageVector.h) already copies only the
  actual received length, clamped to it.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_SIG_MAX_LEN / UBX_NAV_SIG_MAX_BLOCKS

class ubxNAVSIG : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_SIG;

    const char classStr[4] = "NAV";
    const char idStr[4] = "SIG";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_SIG_MAX_LEN; // Maximum payload length - see above
    const uint8_t numCallbackCopies = 1;

    // Header field table - the 8 bytes that precede the repeated blocks. Read with the ordinary
    // getUbxMessageField() / getUbxMessageFieldCallback(), exactly like any other message.
    static const uint8_t numFields = 3;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_SIG_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_SIG_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_SIG_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_SIG_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"version", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},
        {"numSigs", ubxDataType8bit(UBX_CFG_U1), 5, -1, -1}};
        // reserved0[2] at bytes 6-7 is not exposed

    // Block field table - one entry per field in a SINGLE 16-byte per-signal block. Offsets are
    // relative to the start of the block, not the message - see UBX_NAV_SIG_block_t in
    // u-blox_structs.h. Read with getUbxMessageBlockField() / getUbxMessageBlockFieldCallback(),
    // passing the block index (0..numSigs-1) returned by the "numSigs" header field above.
    static const uint8_t numBlockFields = 19;
    static const uint16_t blockHeaderLength = 8; // Bytes before the first block
    static const uint16_t blockLength = 16;      // Bytes per block

    const ubxField ubxBlockFields[numBlockFields] = {
        {"gnssId", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"svId", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},
        {"sigId", ubxDataType8bit(UBX_CFG_U1), 2, -1, -1},
        {"freqId", ubxDataType8bit(UBX_CFG_U1), 3, -1, -1},
        {"prRes", ubxDataType8bit(UBX_CFG_I2), 4, -1, -1},
        {"cno", ubxDataType8bit(UBX_CFG_U1), 6, -1, -1},
        {"qualityInd", ubxDataType8bit(UBX_CFG_U1), 7, -1, -1},
        {"corrSource", ubxDataType8bit(UBX_CFG_U1), 8, -1, -1},
        {"ionoModel", ubxDataType8bit(UBX_CFG_U1), 9, -1, -1},
        {"sigFlags", ubxDataType8bit(UBX_CFG_X2), 10, -1, -1},
        {"health", ubxDataType8bit(UBX_CFG_U1), 10, 0, 2},
        {"prSmoothed", ubxDataType8bit(UBX_CFG_L), 10, 2, 1},
        {"prUsed", ubxDataType8bit(UBX_CFG_L), 10, 3, 1},
        {"crUsed", ubxDataType8bit(UBX_CFG_L), 10, 4, 1},
        {"doUsed", ubxDataType8bit(UBX_CFG_L), 10, 5, 1},
        {"prCorrUsed", ubxDataType8bit(UBX_CFG_L), 10, 6, 1},
        {"crCorrUsed", ubxDataType8bit(UBX_CFG_L), 10, 7, 1},
        {"doCorrUsed", ubxDataType8bit(UBX_CFG_L), 10, 8, 1},
        {"authStatus", ubxDataType8bit(UBX_CFG_L), 10, 9, 1}};
        // reserved1[4] (bytes 12-15) is not exposed

    /**
     * @brief Construct a new ubxNAVSIG object and register it with the UBX message registry.
     *
     * UBX-NAV-SIG: Signal information. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVSIG(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_NAV_SIG_MAX_BLOCKS);
    }
};

ubxRegisterMessage(ubxNAVSIG);
