/*
  ubxRXMQZSSL6.h

  v4 scaffolding: UBX-RXM-QZSSL6 (0x02 0x73) - QZSS L6 raw data, produced by a NEO-D9C. Implemented
  as its own Class, per AGENTS.md "Implement each UBX message in its own Class", treated as a
  fixed-length message with an opaque repeated-byte payload - the same treatment RXM-PMP (Phase 32)
  gave its userData. See u-blox_structs.h's UBX_RXM_QZSSL6_data_t and the u-blox D9-QZS 1.01
  Interface Description (UBX-21031777-QZS), which was consulted for this message's exact field
  layout and bit-field breakdown.

  QZSSL6 cannot be polled - it is "Output" only, same convention as RXM-PMP/ESF-RAW (no
  getRXMQZSSL6() wrapper; the old v3 API never had one either - it only ever offered a raw-message
  callback, setRXMQZSSL6messageCallbackPtr, no field-based one at all). The NEO-D9C outputs it by
  default on all ports; per the old v3 code's own comment, it does not support UBX-CFG-MSG, only
  UBX-CFG-VALSET (setVal8()/setCfgValset(), which is what this library always uses to write
  msgOutKeys, so this is unaffected).

  UNLIKE RXM-PMP: QZSSL6's payload is FIXED length, not variable. The Interface Description gives a
  single, unconditional 264-byte payload (a 14-byte header, then a fixed 250-byte msgBytes array),
  with no byte-count field anywhere in the header. So, matching ESF-RAW's precedent (Phase 31) for
  a message with no block-count field at all, blockCountField is left at its default nullptr here
  too - getUbxMessageBlockCount()/...Callback() fall back to the actual-length-derived count, which
  for this message always comes out to the full 250 (264 - 14 header bytes, / 1 byte per block),
  matching maxBlocks exactly, since the length never actually varies.

  IMPORTANT, PER THE USER'S EXPLICIT INSTRUCTION: numCallbackCopies = UBX_RXM_QZSSL6_NUM_CHANNELS
  (2), NOT the ordinary single-slot "latest wins" value used by every other output-only status-like
  message registered so far (MON-COMMS/SEC-SIG/RXM-PMP/ESF-STATUS). QZSSL6 messages are output two
  at a time - one per L6 reception channel (Channel A / Channel B, decoded by the chName sub-field
  below) - so a single callback slot would let the second message of a pair silently overwrite the
  first before checkCallbacks() gets a chance to drain it. This gives QZSSL6 the same ring-buffered
  callback delivery already used for burst messages like RXM-SFRBX/ESF-MEAS, just with a ring of
  exactly 2 rather than a larger burst-sized number - and, unlike those two messages' buffer
  counts, this is an exact instruction (there are always exactly 2 channels), not an estimate that
  might need raising after real traffic.

  msgBytes (the raw QZSS L6 payload itself, whose format is defined by a different specification,
  IS-QZSS-L6-001, that this repo does not otherwise model) is opaque payload data, exactly like
  RXM-PMP's userData, so it is modelled the same way: 0..UBX_RXM_QZSSL6_DATALEN (250) repeated
  1-byte "blocks" (blockLength = 1) rather than a struct of named fields.

  chInfo (a 2-byte X2 header field) packs four sub-fields, per the Interface Description's bit
  breakdown: chn (bits 9:8, receiver channel 0 or 1), msgName (bit 10, 0=L6D/1=L6E), errStatus
  (bits 13:12, 0=unknown/1=error-free/2=erroneous), chName (bits 15:14, channel name 0=A/1=B). All
  four sub-fields share chInfo's own byte offset (10) as their startByte, with startBit set to
  their bit position within the 2-byte field (8/10/12/14) - ubxMessage::extractBits() already
  supports a startBit beyond the first byte, reading as many little-endian bytes as
  startBit+bitWidth spans (the same mechanism SEC-SIG's 24-bit centFreq (Phase 28) relies on, just
  spanning via a startBit offset here instead of a wide bitWidth). The raw chInfo value itself is
  also exposed as a plain X2 field, for a caller that wants to decode it a different way. Following
  the same single-bit-width convention already used for SEC-SIG's sigSecFlags sub-fields, the
  1-bit msgName sub-field is tagged L (bool); the 2-bit chn/errStatus/chName sub-fields are tagged
  U1.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_RXM_QZSSL6_MAX_LEN / UBX_RXM_QZSSL6_DATALEN / UBX_RXM_QZSSL6_NUM_CHANNELS

class ubxRXMQZSSL6 : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_RXM;
    const uint8_t ID = UBX_RXM_QZSSL6;

    const char classStr[4] = "RXM";
    const char idStr[7] = "QZSSL6";

    const uint16_t messageLength = UBX_RXM_QZSSL6_MAX_LEN;      // Fixed payload length (264 bytes) - see the file header comment above
    const uint8_t numCallbackCopies = UBX_RXM_QZSSL6_NUM_CHANNELS; // 2 - one ring slot per L6 reception channel, per explicit instruction, see the file header comment above

    // Header field table - the 14-byte fixed header, read with the ordinary
    // getUbxMessageField()/getUbxMessageFieldCallback().
    static const uint8_t numFields = 11;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_RXM_QZSSL6_I2C, UBLOX_CFG_MSGOUT_UBX_RXM_QZSSL6_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_RXM_QZSSL6_UART1, UBLOX_CFG_MSGOUT_UBX_RXM_QZSSL6_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},      // Message version (0x01)
        {"svId", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},         // Satellite identifier
        {"cno", ubxDataType8bit(UBX_CFG_U2), 2, -1, -1},          // Mean C/N0, scale 2^-8 dBHz - raw, unscaled value stored, same convention as every other scaled field in this library
        {"timeTag", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},      // Local time tag at message start (ms)
        {"groupDelay", ubxDataType8bit(UBX_CFG_U1), 8, -1, -1},   // L6 group delay w.r.t. L2 (ns)
        {"bitErrCorr", ubxDataType8bit(UBX_CFG_U1), 9, -1, -1},   // Number of bit errors corrected by the Reed-Solomon decoder
        {"chInfo", ubxDataType8bit(UBX_CFG_X2), 10, -1, -1},      // Raw receiver-channel info word - see the four sub-fields below for the decoded bits
        {"chn", ubxDataType8bit(UBX_CFG_U1), 10, 8, 2},           // chInfo bits 9:8 - receiver channel (0 or 1)
        {"msgName", ubxDataType8bit(UBX_CFG_L), 10, 10, 1},       // chInfo bit 10 - message type: 0 = L6D, 1 = L6E
        {"errStatus", ubxDataType8bit(UBX_CFG_U1), 10, 12, 2},    // chInfo bits 13:12 - 0 = unknown, 1 = error-free, 2 = erroneous
        {"chName", ubxDataType8bit(UBX_CFG_U1), 10, 14, 2}};      // chInfo bits 15:14 - channel name: 0 = A, 1 = B
        // reserved0[2] (bytes 12-13) is not exposed

    // Block field table - msgBytes is opaque QZSS L6 payload, not a struct of named fields, so it
    // is modelled as 0..UBX_RXM_QZSSL6_DATALEN repeated single-byte "blocks" (blockLength = 1),
    // same treatment as RXM-PMP's userData - see the file header comment above. Read with
    // getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing a block index up to (but
    // not including) getUbxMessageBlockCount()/...Callback()'s count (always 250 for this fixed-
    // length message).
    static const uint8_t numBlockFields = 1;
    static const uint16_t blockHeaderLength = 14; // Bytes before msgBytes[0]
    static const uint16_t blockLength = 1;        // One byte per msgBytes "block"

    const ubxField ubxBlockFields[numBlockFields] = {
        {"msgBytes", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1}}; // One raw QZSS L6 payload byte

    /**
     * @brief Construct a new ubxRXMQZSSL6 object and register it with the UBX message registry.
     *
     * UBX-RXM-QZSSL6: QZSS L6 data (NEO-D9C). Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxRXMQZSSL6(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_RXM_QZSSL6_DATALEN,
                   nullptr); // blockCountField - none exists; getBlockCount() falls back to the actual-length-derived count, see the file header comment above
    }
};

ubxRegisterMessage(ubxRXMQZSSL6);
