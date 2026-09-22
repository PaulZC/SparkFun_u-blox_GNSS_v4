/*
  ubxRXMMEASX.h

  v4 scaffolding: UBX-RXM-MEASX - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Like NAV-SAT/NAV-SIG (see ubxNAVSAT.h/
  ubxNAVSIG.h), MEASX's payload is variable length: a fixed 44-byte header (TOW/accuracy fields
  for GPS/GLONASS/BeiDou/QZSS, numSV, flags) followed by 0..UBX_RXM_MEASX_MAX_BLOCKS
  identically-shaped 24-byte per-satellite blocks - see u-blox_structs.h's
  UBX_RXM_MEASX_header_t/UBX_RXM_MEASX_block_t and AGENTS.md "Adding the variable-length UBX
  messages". Same mechanism as NAV-SAT/NAV-SIG, just a longer/different-shaped header and block -
  nothing new needed designing here.

  'ubxFields' below describes the header only, read with the ordinary
  getUbxMessageField()/getUbxMessageFieldCallback(). 'flags' also has one named sub-bitfield
  ("towSet", 2 bits) - the first header-level (rather than block-level) sub-bitfield exercised by
  any variable-length message so far, though the mechanism (extractFieldFrom() with a sub-field
  startBit/bitWidth) is exactly the same one NAV-SAT/NAV-SIG already exercise at the block level.
  'ubxBlockFields' describes one repeated block, with offsets relative to the START OF THE BLOCK
  (not the message) - read with getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(),
  passing the block index (0..numSV-1) returned by the "numSV" header field above.

  messageLength is set to UBX_RXM_MEASX_MAX_LEN - the MAXIMUM possible payload, not a fixed wire
  size - exactly as for NAV-SAT/NAV-SIG. storePayload() (in ubxMessageVector.h) already copies
  only the actual received length, clamped to it.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_RXM_MEASX_MAX_LEN / UBX_RXM_MEASX_MAX_BLOCKS

class ubxRXMMEASX : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_RXM;
    const uint8_t ID = UBX_RXM_MEASX;

    const char classStr[4] = "RXM";
    const char idStr[6] = "MEASX";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_RXM_MEASX_MAX_LEN; // Maximum payload length - see above
    const uint8_t numCallbackCopies = 1;

    // Header field table - the 44 bytes that precede the repeated blocks. Read with the ordinary
    // getUbxMessageField() / getUbxMessageFieldCallback(), exactly like any other message.
    static const uint8_t numFields = 12;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_RXM_MEASX_I2C, UBLOX_CFG_MSGOUT_UBX_RXM_MEASX_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_RXM_MEASX_UART1, UBLOX_CFG_MSGOUT_UBX_RXM_MEASX_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        // reserved1[3] at bytes 1-3 is not exposed
        {"gpsTOW", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"gloTOW", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},
        {"bdsTOW", ubxDataType8bit(UBX_CFG_U4), 12, -1, -1},
        // reserved2[4] at bytes 16-19 is not exposed
        {"qzssTOW", ubxDataType8bit(UBX_CFG_U4), 20, -1, -1},
        {"gpsTOWacc", ubxDataType8bit(UBX_CFG_U2), 24, -1, -1},
        {"gloTOWacc", ubxDataType8bit(UBX_CFG_U2), 26, -1, -1},
        {"bdsTOWacc", ubxDataType8bit(UBX_CFG_U2), 28, -1, -1},
        // reserved3[2] at bytes 30-31 is not exposed
        {"qzssTOWacc", ubxDataType8bit(UBX_CFG_U2), 32, -1, -1},
        {"numSV", ubxDataType8bit(UBX_CFG_U1), 34, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 35, -1, -1},
        {"towSet", ubxDataType8bit(UBX_CFG_U1), 35, 0, 2}}; // TOW set: 0 = no, 1 or 2 = yes
        // reserved4[8] at bytes 36-43 is not exposed

    // Block field table - one entry per field in a SINGLE 24-byte per-satellite block. Offsets are
    // relative to the start of the block, not the message - see UBX_RXM_MEASX_block_t in
    // u-blox_structs.h. Read with getUbxMessageBlockField() / getUbxMessageBlockFieldCallback(),
    // passing the block index (0..numSV-1) returned by the "numSV" header field above.
    static const uint8_t numBlockFields = 11;
    static const uint16_t blockHeaderLength = 44; // Bytes before the first block
    static const uint16_t blockLength = 24;       // Bytes per block

    const ubxField ubxBlockFields[numBlockFields] = {
        {"gnssId", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"svId", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},
        {"cNo", ubxDataType8bit(UBX_CFG_U1), 2, -1, -1},
        {"mpathIndic", ubxDataType8bit(UBX_CFG_U1), 3, -1, -1},
        {"dopplerMS", ubxDataType8bit(UBX_CFG_I4), 4, -1, -1},
        {"dopplerHz", ubxDataType8bit(UBX_CFG_I4), 8, -1, -1},
        {"wholeChips", ubxDataType8bit(UBX_CFG_U2), 12, -1, -1},
        {"fracChips", ubxDataType8bit(UBX_CFG_U2), 14, -1, -1},
        {"codePhase", ubxDataType8bit(UBX_CFG_U4), 16, -1, -1},
        {"intCodePhase", ubxDataType8bit(UBX_CFG_U1), 20, -1, -1},
        {"pseuRangeRMSErr", ubxDataType8bit(UBX_CFG_U1), 21, -1, -1}};
        // reserved5[2] (bytes 22-23) is not exposed

    /**
     * @brief Construct a new ubxRXMMEASX object and register it with the UBX message registry.
     *
     * UBX-RXM-MEASX: Satellite measurements for RRLP. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxRXMMEASX(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_RXM_MEASX_MAX_BLOCKS);
    }
};

ubxRegisterMessage(ubxRXMMEASX);
