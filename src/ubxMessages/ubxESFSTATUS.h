/*
  ubxESFSTATUS.h

  v4 scaffolding: UBX-ESF-STATUS - implemented as its own Class, per AGENTS.md "Implement each UBX
  message in its own Class". Like NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX/RXM-SFRBX/MON-COMMS/SEC-SIG/
  ESF-MEAS (see ubxNAVSAT.h etc.), ESF-STATUS's payload is variable length: a fixed 16-byte header
  (iTOW, version, reserved1[7], fusionMode, reserved2[2], numSens) followed by 0..DEF_NUM_SENS
  identically-shaped 4-byte per-sensor status blocks - see u-blox_structs.h's
  UBX_ESF_STATUS_data_t/UBX_ESF_STATUS_sensorStatus_t and AGENTS.md "Adding the variable-length
  UBX messages". Nothing new needed designing for the variable-length shape itself - same
  mechanism as MON-COMMS/SEC-SIG.

  'ubxFields' below describes the header only, read with the ordinary
  getUbxMessageField()/getUbxMessageFieldCallback(). 'ubxBlockFields' describes one repeated
  4-byte per-sensor status block, with offsets relative to the START OF THE BLOCK (not the
  message) - read with getUbxMessageBlockField()/getUbxMessageBlockFieldCallback().

  "numSens" is a genuine, documented header count field (unlike ESF-MEAS's numMeas, which u-blox
  itself flags as unreliable) - following the same convention as MON-COMMS's "nPorts" and
  SEC-SIG's "jamNumCentFreqs", the intended way to bound a block loop is to read "numSens" directly
  and loop with that. 'blockCountField' is left at its default (nullptr) here, matching those two
  messages - this class does not need ESF-MEAS's defensive cross-check machinery. (As a side
  effect of the actual-length-only fallback added for ESF-RAW - see ubxMessage::getBlockCount() -
  getUbxMessageBlockCount()/...Callback() also happen to work for this message, and should agree
  with "numSens" under normal circumstances; either way of reading the block count is fine here.)

  numCallbackCopies is 1 - like MON-COMMS/SEC-SIG, ESF-STATUS is an ordinary periodic status
  message (polled or output at a configured rate), not a message that arrives in back-to-back
  bursts, so the single-slot "latest wins" behavior used by every other registered message
  (other than RXM-SFRBX/ESF-MEAS) is the correct choice here too.

  Each per-sensor block exposes both the raw status byte (e.g. "sensStatus1") and its individual
  bit-packed sub-fields (e.g. "type"/"used"/"ready") - the same convention MON-COMMS uses for
  "txErrors" and SEC-SIG uses for "sigSecFlags".

  messageLength is set to UBX_ESF_STATUS_MAX_LEN - the MAXIMUM possible payload (16-byte header +
  all DEF_NUM_SENS blocks), not a fixed wire size - exactly as for the other variable-length
  messages. storePayload() (in ubxMessageVector.h) already copies only the actual received
  length, clamped to it.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_ESF_STATUS_MAX_LEN / DEF_NUM_SENS

class ubxESFSTATUS : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_ESF;
    const uint8_t ID = UBX_ESF_STATUS;

    const char classStr[4] = "ESF";
    const char idStr[7] = "STATUS";

    const uint8_t supportedVersions = 2; // "version" (0x02 for this version) - see UBX_ESF_STATUS_data_t

    const uint16_t messageLength = UBX_ESF_STATUS_MAX_LEN; // Maximum payload length (header + all blocks) - see above
    const uint8_t numCallbackCopies = 1;

    // Header field table - the 16 bytes that precede the repeated per-sensor blocks. Read with
    // the ordinary getUbxMessageField()/getUbxMessageFieldCallback(), exactly like any other
    // message.
    static const uint8_t numFields = 4;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_ESF_STATUS_I2C, UBLOX_CFG_MSGOUT_UBX_ESF_STATUS_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_ESF_STATUS_UART1, UBLOX_CFG_MSGOUT_UBX_ESF_STATUS_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},        // GPS time of week of the HNR epoch: ms
        {"version", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},     // Message version (0x02 for this version)
        // reserved1[7] at bytes 5-11 is not exposed
        {"fusionMode", ubxDataType8bit(UBX_CFG_U1), 12, -1, -1}, // 0=Init, 1=Fusion, 2=Suspended fusion, 3=Disabled fusion
        // reserved2[2] at bytes 13-14 is not exposed
        {"numSens", ubxDataType8bit(UBX_CFG_U1), 15, -1, -1}};   // Number of sensors - read this directly to bound the block loop, see the file header comment above

    // Block field table - one entry per field in a SINGLE 4-byte per-sensor status block. Offsets
    // are relative to the start of the block, not the message - see UBX_ESF_STATUS_sensorStatus_t
    // in u-blox_structs.h. Read with getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(),
    // passing the block index (0..numSens-1) returned by the "numSens" header field above.
    static const uint8_t numBlockFields = 13;
    static const uint16_t blockHeaderLength = 16; // Bytes before the first block
    static const uint16_t blockLength = 4;         // Bytes per block

    const ubxField ubxBlockFields[numBlockFields] = {
        {"sensStatus1", ubxDataType8bit(UBX_CFG_X1), 0, -1, -1},
        {"type", ubxDataType8bit(UBX_CFG_U1), 0, 0, 6},   // Sensor data type
        {"used", ubxDataType8bit(UBX_CFG_L), 0, 6, 1},    // Sensor data is used for the current fusion solution
        {"ready", ubxDataType8bit(UBX_CFG_L), 0, 7, 1},   // Sensor is set up but not (yet) used for the fusion solution
        {"sensStatus2", ubxDataType8bit(UBX_CFG_X1), 1, -1, -1},
        {"calibStatus", ubxDataType8bit(UBX_CFG_U1), 1, 0, 2}, // 00=not calibrated, 01=calibrating, 10/11=calibrated
        {"timeStatus", ubxDataType8bit(UBX_CFG_U1), 1, 2, 2},  // 00=no data, 01=1st byte used to tag, 10=event input, 11=time tag provided
        {"freq", ubxDataType8bit(UBX_CFG_U1), 2, -1, -1},      // Observation frequency: Hz
        {"faults", ubxDataType8bit(UBX_CFG_X1), 3, -1, -1},
        {"badMeas", ubxDataType8bit(UBX_CFG_L), 3, 0, 1},      // Bad measurements detected
        {"badTTag", ubxDataType8bit(UBX_CFG_L), 3, 1, 1},      // Bad measurement time-tags detected
        {"missingMeas", ubxDataType8bit(UBX_CFG_L), 3, 2, 1},  // Missing or time-misaligned measurements detected
        {"noisyMeas", ubxDataType8bit(UBX_CFG_L), 3, 3, 1}};   // High measurement noise-level detected

    ubxESFSTATUS(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   DEF_NUM_SENS);
        // blockCountField deliberately left at its default (nullptr) - "numSens" is a genuine,
        // documented count field, same convention as MON-COMMS/SEC-SIG - see the file header
        // comment above.
    }
};

ubxRegisterMessage(ubxESFSTATUS);
