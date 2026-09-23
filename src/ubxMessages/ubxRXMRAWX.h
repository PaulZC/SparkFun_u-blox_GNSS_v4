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
 * @file ubxRXMRAWX.h
 */

 /*
  v4 scaffolding: UBX-RXM-RAWX - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Like NAV-SAT/NAV-SIG/RXM-MEASX (see
  ubxNAVSAT.h/ubxNAVSIG.h/ubxRXMMEASX.h), RAWX's payload is variable length: a fixed 16-byte
  header (rcvTow, week, leapS, numMeas, recStat, version) followed by 0..UBX_RXM_RAWX_MAX_BLOCKS
  identically-shaped 32-byte per-measurement blocks - see u-blox_structs.h's
  UBX_RXM_RAWX_header_t/UBX_RXM_RAWX_block_t and AGENTS.md "Adding the variable-length UBX
  messages". Same mechanism as NAV-SAT/NAV-SIG - nothing new needed designing for the
  variable-length shape itself.

  New territory for THIS message specifically: rcvTow/prMes/cpMes are 8-byte IEEE-754 doubles and
  doMes is a 4-byte IEEE-754 float - the first field table anywhere in this codebase to use the
  UBX_CFG_R8/UBX_CFG_R4 data types. extractFieldFrom() (ubxMessage.h) already has working R4/R8
  cases (byte-reassemble little-endian, then memcpy into value->R4/R8), they just haven't been
  exercised by any registered message until now.

  'ubxFields' below describes the header only, read with the ordinary
  getUbxMessageField()/getUbxMessageFieldCallback(). 'recStat' also has two named 1-bit
  sub-bitfields (leapSec, clkReset). 'ubxBlockFields' describes one repeated block, with offsets
  relative to the START OF THE BLOCK (not the message) - read with
  getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing the block index
  (0..numMeas-1) returned by the "numMeas" header field above. 'trkStat' has four named 1-bit
  sub-bitfields (prValid, cpValid, halfCyc, subHalfCyc).

  messageLength is set to UBX_RXM_RAWX_MAX_LEN - the MAXIMUM possible payload, not a fixed wire
  size - exactly as for NAV-SAT/NAV-SIG/RXM-MEASX. storePayload() (in ubxMessageVector.h) already
  copies only the actual received length, clamped to it.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_RXM_RAWX_MAX_LEN / UBX_RXM_RAWX_MAX_BLOCKS

class ubxRXMRAWX : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_RXM;
    const uint8_t ID = UBX_RXM_RAWX;

    const char classStr[4] = "RXM";
    const char idStr[5] = "RAWX";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_RXM_RAWX_MAX_LEN; // Maximum payload length - see above
    // The ZED-F9P needs at least 3 callback copies when running at 20Hz on SPI
    // Even then, some messages are skipped. For best results, if you have RAM to spare, set this to 4
    const uint8_t numCallbackCopies = 1;

    // Header field table - the 16 bytes that precede the repeated blocks. Read with the ordinary
    // getUbxMessageField() / getUbxMessageFieldCallback(), exactly like any other message.
    static const uint8_t numFields = 8;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_RXM_RAWX_I2C, UBLOX_CFG_MSGOUT_UBX_RXM_RAWX_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_RXM_RAWX_UART1, UBLOX_CFG_MSGOUT_UBX_RXM_RAWX_UART2};

    const ubxField ubxFields[numFields] = {
        {"rcvTow", ubxDataType8bit(UBX_CFG_R8), 0, -1, -1}, // Measurement time of week in receiver local time: s
        {"week", ubxDataType8bit(UBX_CFG_U2), 8, -1, -1},
        {"leapS", ubxDataType8bit(UBX_CFG_I1), 10, -1, -1},
        {"numMeas", ubxDataType8bit(UBX_CFG_U1), 11, -1, -1},
        {"recStat", ubxDataType8bit(UBX_CFG_X1), 12, -1, -1},
        {"leapSec", ubxDataType8bit(UBX_CFG_L), 12, 0, 1}, // 1 = Leap seconds have been determined
        {"clkReset", ubxDataType8bit(UBX_CFG_L), 12, 1, 1}, // 1 = Clock reset applied
        {"version", ubxDataType8bit(UBX_CFG_U1), 13, -1, -1}};
        // reserved1[2] at bytes 14-15 is not exposed

    // Block field table - one entry per field in a SINGLE 32-byte per-measurement block. Offsets
    // are relative to the start of the block, not the message - see UBX_RXM_RAWX_block_t in
    // u-blox_structs.h. Read with getUbxMessageBlockField() / getUbxMessageBlockFieldCallback(),
    // passing the block index (0..numMeas-1) returned by the "numMeas" header field above.
    static const uint8_t numBlockFields = 17;
    static const uint16_t blockHeaderLength = 16; // Bytes before the first block
    static const uint16_t blockLength = 32;       // Bytes per block

    const ubxField ubxBlockFields[numBlockFields] = {
        {"prMes", ubxDataType8bit(UBX_CFG_R8), 0, -1, -1},  // Pseudorange measurement: m
        {"cpMes", ubxDataType8bit(UBX_CFG_R8), 8, -1, -1},  // Carrier phase measurement: cycles
        {"doMes", ubxDataType8bit(UBX_CFG_R4), 16, -1, -1}, // Doppler measurement: Hz
        {"gnssId", ubxDataType8bit(UBX_CFG_U1), 20, -1, -1},
        {"svId", ubxDataType8bit(UBX_CFG_U1), 21, -1, -1},
        {"sigId", ubxDataType8bit(UBX_CFG_U1), 22, -1, -1},
        {"freqId", ubxDataType8bit(UBX_CFG_U1), 23, -1, -1},
        {"lockTime", ubxDataType8bit(UBX_CFG_U2), 24, -1, -1},
        {"cno", ubxDataType8bit(UBX_CFG_U1), 26, -1, -1},
        {"prStdev", ubxDataType8bit(UBX_CFG_U1), 27, -1, -1},
        {"cpStdev", ubxDataType8bit(UBX_CFG_U1), 28, -1, -1},
        {"doStdev", ubxDataType8bit(UBX_CFG_U1), 29, -1, -1},
        {"trkStat", ubxDataType8bit(UBX_CFG_X1), 30, -1, -1},
        {"prValid", ubxDataType8bit(UBX_CFG_L), 30, 0, 1},
        {"cpValid", ubxDataType8bit(UBX_CFG_L), 30, 1, 1},
        {"halfCyc", ubxDataType8bit(UBX_CFG_L), 30, 2, 1},
        {"subHalfCyc", ubxDataType8bit(UBX_CFG_L), 30, 3, 1}};
        // reserved2 (byte 31) is not exposed

    /**
     * @brief Construct a new ubxRXMRAWX object and register it with the UBX message registry.
     *
     * UBX-RXM-RAWX: Multi-GNSS raw measurement data. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxRXMRAWX(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_RXM_RAWX_MAX_BLOCKS);
    }
};

ubxRegisterMessage(ubxRXMRAWX);
