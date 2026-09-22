/*
  ubxMONCOMMS.h

  v4 scaffolding: UBX-MON-COMMS - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Like NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX/RXM-SFRBX
  (see ubxNAVSAT.h etc.), MON-COMMS's payload is variable length: a fixed 8-byte header (version,
  nPorts, txErrors, reserved0, protIds[4]) followed by 0..UBX_MON_COMMS_MAX_PORTS identically-shaped
  40-byte per-port blocks - see u-blox_structs.h's UBX_MON_COMMS_header_t/UBX_MON_COMMS_port_t and
  AGENTS.md "Adding the variable-length UBX messages". Nothing new needed designing for the
  variable-length shape itself - same mechanism as the messages above.

  'ubxFields' below describes the header only, read with the ordinary
  getUbxMessageField()/getUbxMessageFieldCallback(). 'ubxBlockFields' describes one repeated
  40-byte port block, with offsets relative to the START OF THE BLOCK (not the message) - read with
  getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing the block index
  (0..nPorts-1) returned by the "nPorts" header field above.

  numCallbackCopies is 1 - unlike RXM-SFRBX, MON-COMMS is an ordinary periodic status/diagnostic
  message (polled or output at a configured rate), not a message that arrives in back-to-back
  bursts, so the single-slot "latest wins" behavior used by every other registered message is
  the correct choice here too.

  messageLength is set to UBX_MON_COMMS_MAX_LEN - the MAXIMUM possible payload (all
  UBX_MON_COMMS_MAX_PORTS ports present), not a fixed wire size - exactly as for the other
  variable-length messages. storePayload() (in ubxMessageVector.h) already copies only the actual
  received length, clamped to it.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_MON_COMMS_MAX_LEN / UBX_MON_COMMS_MAX_PORTS

class ubxMONCOMMS : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_MON;
    const uint8_t ID = UBX_MON_COMMS;

    const char classStr[4] = "MON";
    const char idStr[6] = "COMMS";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_MON_COMMS_MAX_LEN; // Maximum payload length - see above
    const uint8_t numCallbackCopies = 1;

    // Header field table - the 8 bytes that precede the repeated port blocks. Read with the
    // ordinary getUbxMessageField() / getUbxMessageFieldCallback(), exactly like any other
    // message.
    static const uint8_t numFields = 10;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_MON_COMMS_I2C, UBLOX_CFG_MSGOUT_UBX_MON_COMMS_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_MON_COMMS_UART1, UBLOX_CFG_MSGOUT_UBX_MON_COMMS_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"nPorts", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},
        {"txErrors", ubxDataType8bit(UBX_CFG_X1), 2, -1, -1},
        {"mem", ubxDataType8bit(UBX_CFG_L), 2, 0, 1},   // Memory allocation error
        {"alloc", ubxDataType8bit(UBX_CFG_L), 2, 1, 1}, // Allocation error (TX buffer full)
        {"outputPort", ubxDataType8bit(UBX_CFG_U1), 2, 2, 3}, // Output port
        // reserved0 at byte 3 is not exposed
        {"protId0", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1},
        {"protId1", ubxDataType8bit(UBX_CFG_U1), 5, -1, -1},
        {"protId2", ubxDataType8bit(UBX_CFG_U1), 6, -1, -1},
        {"protId3", ubxDataType8bit(UBX_CFG_U1), 7, -1, -1}};
        // protIds[4]: 0=UBX, 1=NMEA, 2=RTCM2, 5=RTCM3, 6=SPARTN, 0xFF=no protocol reported

    // Block field table - one entry per field in a SINGLE 40-byte per-port block. Offsets are
    // relative to the start of the block, not the message - see UBX_MON_COMMS_port_t in
    // u-blox_structs.h. Read with getUbxMessageBlockField() / getUbxMessageBlockFieldCallback(),
    // passing the block index (0..nPorts-1) returned by the "nPorts" header field above.
    static const uint8_t numBlockFields = 15;
    static const uint16_t blockHeaderLength = 8; // Bytes before the first block
    static const uint16_t blockLength = 40;      // Bytes per block

    const ubxField ubxBlockFields[numBlockFields] = {
        {"portId", ubxDataType8bit(UBX_CFG_U2), 0, -1, -1},
        {"txPending", ubxDataType8bit(UBX_CFG_U2), 2, -1, -1},
        {"txBytes", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},
        {"txUsage", ubxDataType8bit(UBX_CFG_U1), 8, -1, -1},
        {"txPeakUsage", ubxDataType8bit(UBX_CFG_U1), 9, -1, -1},
        {"rxPending", ubxDataType8bit(UBX_CFG_U2), 10, -1, -1},
        {"rxBytes", ubxDataType8bit(UBX_CFG_U4), 12, -1, -1},
        {"rxUsage", ubxDataType8bit(UBX_CFG_U1), 16, -1, -1},
        {"rxPeakUsage", ubxDataType8bit(UBX_CFG_U1), 17, -1, -1},
        {"overrunErrs", ubxDataType8bit(UBX_CFG_U2), 18, -1, -1},
        {"msgs0", ubxDataType8bit(UBX_CFG_U2), 20, -1, -1},
        {"msgs1", ubxDataType8bit(UBX_CFG_U2), 22, -1, -1},
        {"msgs2", ubxDataType8bit(UBX_CFG_U2), 24, -1, -1},
        {"msgs3", ubxDataType8bit(UBX_CFG_U2), 26, -1, -1},
        // reserved1[8] at bytes 28-35 is not exposed
        {"skipped", ubxDataType8bit(UBX_CFG_U4), 36, -1, -1}};
        // msgs0..msgs3 map 1:1 to the header's protId0..protId3 - number of successfully parsed
        // messages for each of the (up to 4) protocols reported in this port's row

    /**
     * @brief Construct a new ubxMONCOMMS object and register it with the UBX message registry.
     *
     * UBX-MON-COMMS: Comm port information. Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxMONCOMMS(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_MON_COMMS_MAX_PORTS);
    }
};

ubxRegisterMessage(ubxMONCOMMS);
