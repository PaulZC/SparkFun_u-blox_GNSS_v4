/*
  ubxMONRF.h

  v4 scaffolding: UBX-MON-RF - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Like MON-COMMS (see ubxMONCOMMS.h), MON-RF's
  payload is variable length: a fixed 4-byte header (version, nBlocks, reserved0[2]) followed by
  0..UBX_MON_RF_MAX_BLOCKS identically-shaped 24-byte per-RF-block blocks - see u-blox_structs.h's
  (removed) UBX_MON_RF_header_t/UBX_MON_RF_block_t and AGENTS.md "Adding the variable-length UBX
  messages". Nothing new needed designing for the variable-length shape itself - same mechanism as
  MON-COMMS/NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX/SEC-SIG.

  'ubxFields' below describes the header only, read with the ordinary
  getUbxMessageField()/getUbxMessageFieldCallback(). 'ubxBlockFields' describes one repeated
  24-byte RF block, with offsets relative to the START OF THE BLOCK (not the message) - read with
  getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing the block index
  (0..nBlocks-1) returned by the "nBlocks" header field above.

  numCallbackCopies is 1 - MON-RF is an ordinary periodic status/diagnostic message (polled or
  output at a configured rate), not a message that arrives in back-to-back bursts, so the
  single-slot "latest wins" behavior used by most registered messages is the correct choice here
  too.

  messageLength is set to UBX_MON_RF_MAX_LEN - the MAXIMUM possible payload (all
  UBX_MON_RF_MAX_BLOCKS blocks present, currently 2: L1 and L2/L5), not a fixed wire size - exactly
  as for the other variable-length messages. storePayload() (in ubxMessageVector.h) already copies
  only the actual received length, clamped to it.

  Replaces the old v3-style getRFinformation(UBX_MON_RF_data_t *data, ...) - which polled with
  packetCfg/sendCommand() and hand-extracted every field - with the usual getMONRF() thin wrapper
  around getUBX(). See u-blox_GNSS.h/.cpp.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_MON_RF_MAX_LEN / UBX_MON_RF_MAX_BLOCKS

class ubxMONRF : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_MON;
    const uint8_t ID = UBX_MON_RF;

    const char classStr[4] = "MON";
    const char idStr[3] = "RF";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_MON_RF_MAX_LEN; // Maximum payload length - see above
    const uint8_t numCallbackCopies = 1;

    // Header field table - the 4 bytes that precede the repeated RF blocks. Read with the
    // ordinary getUbxMessageField() / getUbxMessageFieldCallback(), exactly like any other
    // message.
    static const uint8_t numFields = 4;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_MON_RF_I2C, UBLOX_CFG_MSGOUT_UBX_MON_RF_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_MON_RF_UART1, UBLOX_CFG_MSGOUT_UBX_MON_RF_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"nBlocks", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},
        {"recInf", ubxDataType8bit(UBX_CFG_X1), 2, -1, -1},
        {"msgSource", ubxDataType8bit(UBX_CFG_U1), 2, 0, 2}};
        // reserved0 at byte 3 is not exposed

    // Block field table - one entry per field in a SINGLE 24-byte RF block. Offsets are
    // relative to the start of the block, not the message - see the (removed) UBX_MON_RF_block_t
    // in u-blox_structs.h's history. Read with getUbxMessageBlockField() /
    // getUbxMessageBlockFieldCallback(), passing the block index (0..nBlocks-1) returned by the
    // "nBlocks" header field above.
    static const uint8_t numBlockFields = 14;
    static const uint16_t blockHeaderLength = 4; // Bytes before the first block
    static const uint16_t blockLength = 24;      // Bytes per block

    const ubxField ubxBlockFields[numBlockFields] = {
        {"blockId", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},          // 0 = L1 band, 1 = L2 or L5 band
        {"flags", ubxDataType8bit(UBX_CFG_X1), 1, -1, -1},
        {"jammingState", ubxDataType8bit(UBX_CFG_U1), 1, 0, 2},       // 0=unknown/disabled, 1=ok, 2=warning, 3=critical
        {"antStatus", ubxDataType8bit(UBX_CFG_U1), 2, -1, -1},        // 0=INIT, 1=DONTKNOW, 2=OK, 3=SHORT, 4=OPEN
        {"antPower", ubxDataType8bit(UBX_CFG_U1), 3, -1, -1},         // 0=OFF, 1=ON, 2=DONTKNOW
        {"postStatus", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        // reserved1[4] at bytes 8-11 is not exposed
        {"noisePerMS", ubxDataType8bit(UBX_CFG_U2), 12, -1, -1},
        {"agcCnt", ubxDataType8bit(UBX_CFG_U2), 14, -1, -1},
        {"cwSuppression", ubxDataType8bit(UBX_CFG_U1), 16, -1, -1},
        {"ofsI", ubxDataType8bit(UBX_CFG_I1), 17, -1, -1},
        {"magI", ubxDataType8bit(UBX_CFG_U1), 18, -1, -1},
        {"ofsQ", ubxDataType8bit(UBX_CFG_I1), 19, -1, -1},
        {"magQ", ubxDataType8bit(UBX_CFG_U1), 20, -1, -1},
        {"rfBlockGnssBand", ubxDataType8bit(UBX_CFG_U1), 21, -1, -1}}; // 0=unknown; 1=L1 band; 2=L2 band; 3=L3 band; 4=L5 band
        // reserved2[2] at bytes 22-23 is not exposed

    /**
     * @brief Construct a new ubxMONRF object and register it with the UBX message registry.
     *
     * UBX-MON-RF: RF information. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxMONRF(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_MON_RF_MAX_BLOCKS);
    }
};

ubxRegisterMessage(ubxMONRF);
