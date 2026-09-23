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
 * @file ubxESFMEAS.h
 */

 /*
  v4 scaffolding: UBX-ESF-MEAS - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Like NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX/
  RXM-SFRBX/SEC-SIG (see ubxNAVSAT.h etc.), ESF-MEAS's payload is variable length: a fixed 8-byte
  header (timeTag, flags, id) followed by 0..DEF_MAX_NUM_ESF_MEAS identically-shaped 4-byte
  measurement blocks - see u-blox_structs.h's UBX_ESF_MEAS_data_t and AGENTS.md "Adding the
  variable-length UBX messages".

  Two things are new here, beyond the ordinary variable-length-block mechanism:

  1. The header's own block-count field ("numMeas") is documented by u-blox as "optional, can be
     obtained from message size" - i.e. not guaranteed to be populated correctly by the receiver.
     'blockCountField' below ("numMeas") tells the base class to cross-check it defensively against
     the message's actual received byte length rather than trust it outright - see
     ubxMessage::getBlockCount() and DevUBLOXGNSS::getUbxMessageBlockCount()/...Callback() in
     u-blox_GNSS.cpp.
  2. ESF-MEAS can include an OPTIONAL trailing 4-byte footer group ("calibTtag") after the last real
     measurement block - similar in spirit to the NMEA GSV "footer". Whether the GNSS module ever
     actually outputs it is not clear (u-blox's own note: "calibTtagValid... Always set to zero", so
     that flag bit cannot be used to detect it either) - per AGENTS.md "Adding support for
     ESF-MEAS": "For safety, we should assume that ESF-MEAS output by the GNSS can include
     calibTtag." 'ubxFooterFields' below describes it; its presence in any one particular received
     message is determined defensively from that message's actual byte length (not from a flag bit,
     and not from the footer bytes themselves, which can be stale leftovers from a previous, longer
     message sharing the same storage) - see ubxMessage::extractFooterFieldFrom() and
     DevUBLOXGNSS::getUbxMessageFooterField()/...Callback().

  'ubxFields' below describes the header only, read with the ordinary
  getUbxMessageField()/getUbxMessageFieldCallback(). 'ubxBlockFields' describes one repeated 4-byte
  measurement block, with offsets relative to the START OF THE BLOCK (not the message) - read with
  getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing a block index up to (but not
  including) the DEFENSIVE block count returned by getUbxMessageBlockCount()/...Callback() - NOT the
  raw "numMeas" header field value, for the reason given above. 'ubxFooterFields' describes the
  optional trailing calibTtag group, read with getUbxMessageFooterField()/...Callback().

  dataField is a 24-bit sub-field, the same width as SEC-SIG's centFreq - it needs the FULL 32-bit
  union member to read back correctly (X4/U4, not U1) - see ubxSECSIG.h's file header comment for
  the full explanation of why. dataType (6 bits) fits within a single byte of the union, so U1 is
  correct for it, exactly like SEC-SIG's jamState/spfState sub-fields.

  numCallbackCopies is UBX_ESF_MEAS_CALLBACK_BUFFERS (u-blox_structs.h), the same
  ring-buffered-callback-storage mechanism built for RXM-SFRBX - several ESF-MEAS messages can
  arrive in a group within a single checkUblox() call. Per the Phase 26 precedent with RXM-SFRBX
  (14 buffers assumed in the interface description / v3 library, 50 needed on real ZED-X20P
  hardware - more than 3x), UBX_ESF_MEAS_CALLBACK_BUFFERS should be treated as a starting point,
  not a validated figure - it has NOT yet been checked against real hardware traffic. See AGENTS.md
  "Adding support for RXM-SFRBX" ("Bearing on ESF-MEAS").

  messageLength is set to UBX_ESF_MEAS_MAX_LEN - the MAXIMUM possible payload (8-byte header + 31
  measurement blocks + 4-byte footer), not a fixed wire size - exactly as for the other
  variable-length messages. storePayload() (in ubxMessageVector.h) already copies only the actual
  received length, clamped to it, and records that actual length in _actualLength/
  _callbackActualLength for the defensive block-count/footer logic above to use.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_ESF_MEAS_MAX_LEN / DEF_MAX_NUM_ESF_MEAS / UBX_ESF_MEAS_CALLBACK_BUFFERS

class ubxESFMEAS : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_ESF;
    const uint8_t ID = UBX_ESF_MEAS;

    const char classStr[4] = "ESF";
    const char idStr[5] = "MEAS";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_ESF_MEAS_MAX_LEN; // Maximum payload length (header + all blocks + footer) - see above
    // Multiple buffered slots, not 1 - ESF-MEAS messages can arrive in a back-to-back group within
    // a single checkUblox() call, like RXM-SFRBX. See AGENTS.md "Adding support for RXM-SFRBX" /
    // "Adding support for ESF-MEAS". NOT YET validated against real hardware traffic - see above.
    const uint8_t numCallbackCopies = UBX_ESF_MEAS_CALLBACK_BUFFERS;

    // Header field table - the 8 bytes that precede the repeated measurement blocks. Read with the
    // ordinary getUbxMessageField()/getUbxMessageFieldCallback(), exactly like any other message.
    static const uint8_t numFields = 7;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_ESF_MEAS_I2C, UBLOX_CFG_MSGOUT_UBX_ESF_MEAS_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_ESF_MEAS_UART1, UBLOX_CFG_MSGOUT_UBX_ESF_MEAS_UART2};

    const ubxField ubxFields[numFields] = {
        {"timeTag", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},         // Time tag of measurement generated by external sensor
        {"flags", ubxDataType8bit(UBX_CFG_X2), 4, -1, -1},           // Full 16-bit flags value
        {"timeMarkSent", ubxDataType8bit(UBX_CFG_U1), 4, 0, 2},      // 0 = none, 1 = on Ext0, 2 = on Ext1
        {"timeMarkEdge", ubxDataType8bit(UBX_CFG_L), 4, 2, 1},       // Rising (0) or falling (1) edge of time mark signal
        {"calibTtagValid", ubxDataType8bit(UBX_CFG_L), 4, 3, 1},     // Per u-blox: "Always set to zero" - NOT used to detect the footer, see the file header comment above
        // reserved (bits 4-10) is not exposed
        {"numMeas", ubxDataType8bit(UBX_CFG_U1), 4, 11, 5},          // Header's OWN claimed count - documented as unreliable. Use getUbxMessageBlockCount()/...Callback() instead, not this field, to loop over blocks - see the file header comment above
        {"id", ubxDataType8bit(UBX_CFG_U2), 6, -1, -1}};             // Identification number of data provider

    // Block field table - describes a SINGLE 4-byte measurement block. Offsets are relative to the
    // start of the block, not the message - see UBX_ESF_MEAS_sensorData_t in u-blox_structs.h. Read
    // with getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing a block index up to
    // (but not including) getUbxMessageBlockCount()/...Callback()'s DEFENSIVE count.
    static const uint8_t numBlockFields = 2;
    static const uint16_t blockHeaderLength = 8; // Bytes before the first measurement block
    static const uint16_t blockLength = 4;        // Bytes per measurement block

    const ubxField ubxBlockFields[numBlockFields] = {
        {"dataField", ubxDataType8bit(UBX_CFG_X4), 0, 0, 24}, // See the file header comment above re: tagging a 24-bit sub-field X4/U4, not U1
        {"dataType", ubxDataType8bit(UBX_CFG_U1), 0, 24, 6}}; // 0 = no data; 1..63 = data type

    // Footer field table - the OPTIONAL 4-byte calibTtag group that may follow the last real
    // measurement block. Read with getUbxMessageFooterField()/getUbxMessageFooterFieldCallback(),
    // which return "field not found" for a particular message that did not actually include it -
    // see the file header comment above and ubxMessage::extractFooterFieldFrom().
    static const uint8_t numFooterFields = 1;
    static const uint16_t footerLength = 4; // Bytes in the footer group

    const ubxField ubxFooterFields[numFooterFields] = {
        {"calibTtag", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1}}; // OPTIONAL: Receiver local time calibrated (ms) - offset is relative to the start of the footer

    /**
     * @brief Construct a new ubxESFMEAS object and register it with the UBX message registry.
     *
     * UBX-ESF-MEAS: External sensor fusion measurements. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxESFMEAS(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_ESF_MEAS_MAX_NUM_MEAS,
                   "numMeas", // blockCountField - cross-checked defensively, see the file header comment above
                   (const void *)&ubxFooterFields, numFooterFields, footerLength);
    }
};

ubxRegisterMessage(ubxESFMEAS);
