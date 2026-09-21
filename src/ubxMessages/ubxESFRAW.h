/*
  ubxESFRAW.h

  v4 scaffolding: UBX-ESF-RAW - implemented as its own Class, per AGENTS.md "Implement each UBX
  message in its own Class". Like NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX/RXM-SFRBX/MON-COMMS/SEC-SIG/
  ESF-MEAS (see ubxNAVSAT.h etc.), ESF-RAW's payload is variable length: a 4-byte header (all
  reserved - see below) followed by 0..DEF_NUM_SENS*DEF_MAX_NUM_ESF_RAW_REPEATS identically-shaped
  8-byte raw-sensor-measurement blocks - see u-blox_structs.h's UBX_ESF_RAW_data_t/
  UBX_ESF_RAW_sensorData_t and AGENTS.md "Adding the variable-length UBX messages".

  ESF-RAW is unlike every other variable-length message registered so far: its 4-byte header is
  ENTIRELY reserved - there is no block-count field anywhere in the wire format at all. The old
  v3 struct even says so explicitly: "UBX_ESF_RAW_data_t.numEsfRawBlocks: Note: this is not
  contained in the ESF RAW message. It is calculated from the message length." So there is no
  header field for a caller to read and loop on (unlike MON-COMMS's "nPorts" or SEC-SIG's
  "jamNumCentFreqs") - getUbxMessageBlockCount()/...Callback() (ubxMessage::getBlockCount()) is
  the ONLY way to know how many blocks are actually present in a particular message, and it now
  falls back to computing this purely from the real received byte length when no block-count
  field is set (see the extended comment on getBlockCount() in ubxMessage.h) - this class leaves
  'blockCountField' at its default (nullptr) specifically to use that fallback. numFields is 0 and
  'ubxFields' is passed as nullptr, since there is nothing in the 4-byte header worth exposing as
  a named field.

  Per AGENTS.md/the user's explicit instruction: the u-blox_structs.h comment above
  UBX_ESF_RAW_MAX_LEN notes "the NEO-M8U sends them in sets of ten (i.e. seventy readings per
  message)" - this is about how many BLOCKS one single ESF-RAW message can contain (which
  messageLength/maxBlocks below already size for, unchanged), not about needing multiple
  _callbackStorage ring-buffer copies the way RXM-SFRBX/ESF-MEAS do for a burst of separate
  MESSAGES arriving together. numCallbackCopies is 1 - explicitly instructed - the single-slot
  "latest wins" behavior used by most registered messages is correct here.

  'ubxBlockFields' describes one repeated 8-byte raw-sensor-measurement block, with offsets
  relative to the START OF THE BLOCK (not the message) - read with
  getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing a block index up to (but
  not including) getUbxMessageBlockCount()/...Callback()'s count.

  dataField is a 24-bit sub-field, the same width/tagging concern as ESF-MEAS's dataField and
  SEC-SIG's centFreq - it needs the FULL 32-bit union member to read back correctly (X4/U4, not
  U1) - see ubxSECSIG.h's file header comment for the full explanation of why. dataType here is a
  full 8-bit sub-field (unlike ESF-MEAS's 6-bit dataType), so U1 is correct for it directly.

  messageLength is set to UBX_ESF_RAW_MAX_LEN - the MAXIMUM possible payload (4-byte header + all
  DEF_NUM_SENS*DEF_MAX_NUM_ESF_RAW_REPEATS blocks), not a fixed wire size - exactly as for the
  other variable-length messages. storePayload() (in ubxMessageVector.h) already copies only the
  actual received length, clamped to it, and records that actual length in _actualLength/
  _callbackActualLength for getBlockCount()'s length-only fallback to use.

  ESF-RAW data cannot be polled - it is "Output" only (see the comment above the real ESF-RAW
  section in u-blox_structs.h) - so, unlike ESF-STATUS/MON-COMMS/SEC-SIG, there is no getESFRAW()
  poll wrapper: the old v3 API never had one either, only the automatic/callback path applies.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_ESF_RAW_MAX_LEN / DEF_NUM_SENS / DEF_MAX_NUM_ESF_RAW_REPEATS

class ubxESFRAW : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_ESF;
    const uint8_t ID = UBX_ESF_RAW;

    const char classStr[4] = "ESF";
    const char idStr[4] = "RAW";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_ESF_RAW_MAX_LEN; // Maximum payload length (header + all blocks) - see above
    const uint8_t numCallbackCopies = 1;                // Explicitly instructed - see the file header comment above

    // No header fields at all - the 4-byte header is entirely reserved. See the file header
    // comment above.
    static const uint8_t numFields = 0;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_ESF_RAW_I2C, UBLOX_CFG_MSGOUT_UBX_ESF_RAW_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_ESF_RAW_UART1, UBLOX_CFG_MSGOUT_UBX_ESF_RAW_UART2};

    // Block field table - describes a SINGLE 8-byte raw-sensor-measurement block. Offsets are
    // relative to the start of the block, not the message - see UBX_ESF_RAW_sensorData_t in
    // u-blox_structs.h. Read with getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(),
    // passing a block index up to (but not including) getUbxMessageBlockCount()/...Callback().
    static const uint8_t numBlockFields = 3;
    static const uint16_t blockHeaderLength = 4; // Bytes before the first block (all reserved)
    static const uint16_t blockLength = 8;        // Bytes per block

    const ubxField ubxBlockFields[numBlockFields] = {
        {"dataField", ubxDataType8bit(UBX_CFG_X4), 0, 0, 24}, // See the file header comment above re: tagging a 24-bit sub-field X4/U4, not U1
        {"dataType", ubxDataType8bit(UBX_CFG_U1), 0, 24, 8},  // 0 = no data; 1..255 = data type
        {"sTag", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1}};    // Sensor time tag

    ubxESFRAW(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)nullptr, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   (uint16_t)(UBX_ESF_RAW_MAX_NUM_SENS * UBX_ESF_RAW_MAX_NUM_SENS_REPEATS));
        // blockCountField deliberately left at its default (nullptr) - see the file header
        // comment above and ubxMessage::getBlockCount()'s actual-length-only fallback.
    }
};

ubxRegisterMessage(ubxESFRAW);
