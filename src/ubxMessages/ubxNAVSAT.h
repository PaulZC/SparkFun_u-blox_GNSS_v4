/*
  ubxNAVSAT.h

  v4 scaffolding: UBX-NAV-SAT - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Unlike every other currently-registered message,
  NAV-SAT's payload is variable length: a fixed 8-byte header (iTOW, version, numSvs) followed by
  0..UBX_NAV_SAT_MAX_BLOCKS identically-shaped 12-byte per-SV blocks - see u-blox_structs.h's
  UBX_NAV_SAT_header_t/UBX_NAV_SAT_block_t and AGENTS.md "Adding the variable-length UBX messages".

  'ubxFields' below describes the header only, exactly like every other message's field table -
  read with the ordinary getUbxMessageField()/getUbxMessageFieldCallback(). 'ubxBlockFields'
  describes one repeated block, with offsets relative to the START OF THE BLOCK (not the message) -
  read with the new getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), which take a
  block index (0..numSvs-1) alongside the message pointer and field name.

  messageLength is set to UBX_NAV_SAT_MAX_LEN - the MAXIMUM possible payload, not a fixed wire
  size. initStorage()/initCallbackStorage() allocate this maximum; storePayload() (in
  ubxMessageVector.h) already copies only the actual received length, clamped to it - no change
  was needed there for variable-length support.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_NAV_SAT_MAX_LEN / UBX_NAV_SAT_MAX_BLOCKS

class ubxNAVSAT : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_SAT;

    const char classStr[4] = "NAV";
    const char idStr[4] = "SAT";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_NAV_SAT_MAX_LEN; // Maximum payload length - see above
    const uint8_t numCallbackCopies = 1;

    // Header field table - the 8 bytes that precede the repeated blocks. Read with the ordinary
    // getUbxMessageField() / getUbxMessageFieldCallback(), exactly like any other message.
    static const uint8_t numFields = 3;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_SAT_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_SAT_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_NAV_SAT_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_SAT_UART2};

    const ubxField ubxFields[numFields] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"version", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},
        {"numSvs", ubxDataType8bit(UBX_CFG_U1), 5, -1, -1}};
        // reserved1[2] at bytes 6-7 is not exposed

    // Block field table - one entry per field in a SINGLE 12-byte per-SV block. Offsets are
    // relative to the start of the block, not the message - see UBX_NAV_SAT_block_t in
    // u-blox_structs.h. Read with getUbxMessageBlockField() / getUbxMessageBlockFieldCallback(),
    // passing the block index (0..numSvs-1) returned by the "numSvs" header field above.
    static const uint8_t numBlockFields = 28;
    static const uint16_t blockHeaderLength = 8; // Bytes before the first block
    static const uint16_t blockLength = 12;      // Bytes per block

    const ubxField ubxBlockFields[numBlockFields] = {
        {"gnssId", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"svId", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},
        {"cno", ubxDataType8bit(UBX_CFG_U1), 2, -1, -1},
        {"elev", ubxDataType8bit(UBX_CFG_I1), 3, -1, -1},
        {"azim", ubxDataType8bit(UBX_CFG_I2), 4, -1, -1},
        {"prRes", ubxDataType8bit(UBX_CFG_I2), 6, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X4), 8, -1, -1},
        {"qualityInd", ubxDataType8bit(UBX_CFG_U1), 8, 0, 3},
        {"svUsed", ubxDataType8bit(UBX_CFG_L), 8, 3, 1},
        {"health", ubxDataType8bit(UBX_CFG_U1), 8, 4, 2},
        {"diffCorr", ubxDataType8bit(UBX_CFG_L), 8, 6, 1},
        {"smoothed", ubxDataType8bit(UBX_CFG_L), 8, 7, 1},
        {"orbitSource", ubxDataType8bit(UBX_CFG_U1), 8, 8, 3},
        {"ephAvail", ubxDataType8bit(UBX_CFG_L), 8, 11, 1},
        {"almAvail", ubxDataType8bit(UBX_CFG_L), 8, 12, 1},
        {"anoAvail", ubxDataType8bit(UBX_CFG_L), 8, 13, 1},
        {"aopAvail", ubxDataType8bit(UBX_CFG_L), 8, 14, 1},
        // bit 15 (reserved1) is not exposed
        {"sbasCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 16, 1},
        {"rtcmCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 17, 1},
        {"slasCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 18, 1},
        {"spartnCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 19, 1},
        {"prCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 20, 1},
        {"crCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 21, 1},
        {"doCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 22, 1},
        {"clasCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 23, 1}, // Defined in X20 HPG-2.10
        {"lppCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 24, 1}, // Defined in X20 HPG-2.10
        {"hasCorrUsed", ubxDataType8bit(UBX_CFG_L), 8, 25, 1}}; // Defined in X20 HPG-2.10
        // bits 26-31 (reserved2) are not exposed

    /**
     * @brief Construct a new ubxNAVSAT object and register it with the UBX message registry.
     *
     * UBX-NAV-SAT: Satellite information. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxNAVSAT(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_NAV_SAT_MAX_BLOCKS);
    }
};

ubxRegisterMessage(ubxNAVSAT);
