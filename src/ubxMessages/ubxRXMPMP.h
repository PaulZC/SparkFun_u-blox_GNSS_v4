/*
  ubxRXMPMP.h

  v4 scaffolding: UBX-RXM-PMP (0x02 0x72) - PMP raw data, produced by a NEO-D9S. Implemented as its
  own Class, per AGENTS.md "Implement each UBX message in its own Class", treated as an ordinary
  variable-length message like NAV-SAT/RXM-RAWX/RXM-MEASX/MON-COMMS/SEC-SIG/ESF-RAW (see
  u-blox_structs.h's UBX_RXM_PMP_data_t and AGENTS.md "Adding the variable-length UBX messages").

  PMP cannot be polled - it is "Output" only, same convention as ESF-RAW (no getRXMPMP() wrapper;
  the old v3 API never had one either). The NEO-D9S outputs it by default on all ports; it does NOT
  support UBX-CFG-MSG, only UBX-CFG-VALSET (setVal8()/setCfgValset(), which is what this library
  always uses to write msgOutKeys, so this is unaffected).

  IMPORTANT, DELIBERATE LIMITATION - only message Version 0x01 is correctly modelled:

  UBX-RXM-PMP has two real wire layouts, selected by the "version" byte, and unlike SEC-SIG's
  Version 2 vs Version 3 (Phase 28-29), the two PMP versions do NOT share a byte layout - they
  genuinely disagree on where fecBits/ebno/userData sit:
    - Version 0x00: FIXED length (528 bytes always). userData is a fixed 504 bytes, starting
      immediately after the 20-byte common header (byte 20); fecBits/ebno sit AFTER userData, at
      bytes 524/526. The old v3 code's own comment: "Note: length is variable with version 0x01" -
      i.e. NOT variable with version 0x00.
    - Version 0x01: VARIABLE length (24 + numBytesUserData, up to 528). fecBits/ebno/reserved1 sit
      immediately after the 20-byte common header (bytes 20/22/23); userData is variable-length
      (0..504 bytes, per numBytesUserData), starting at byte 24.
  Byte 2-3 ("numBytesUserData" in the common header) also means something different per version:
  for Version 0x01 it is the real userData byte count; for Version 0x00 it is unused/reserved.

  The v4 field-table mechanism has one fixed byte offset per field - it cannot represent "this
  field moves depending on a runtime byte value" - so this class models ONLY Version 0x01's layout,
  matching the instruction to "treat RXM-PMP like a standard variable-length UBX message": Version
  0x01 is the one that is actually variable-length. A real Version 0x00 message would be misparsed
  by this class (its reserved bytes 2-3 would be read as a bogus userData length, and fecBits/ebno/
  userData would all be read from the wrong offsets) - same category of accepted, deliberate
  limitation as SEC-SIG Version 1 being left unmodelled (Phase 28). Nothing in this repo has
  confirmed which version a real NEO-D9S actually outputs - per the Phase 29 SEC-SIG surprise (a
  receiver's real output can differ from what looks like the "current" documented version), this is
  worth checking against real hardware before trusting this class outright, exactly as flagged in
  claude/v4-migration-status.md.

  userData itself is modelled as 0..UBX_RXM_PMP_MAX_USER_DATA (504) repeated 1-byte "blocks" (see
  ubxBlockFields below) rather than named fields, since it is opaque payload data (PMP correction
  data for the D9S's downstream receiver), not a struct of named values - the same treatment would
  apply to any message whose repeated element is a raw byte rather than a multi-field record.
  "numBytesUserData" is passed as blockCountField, so getUbxMessageBlockCount()/...Callback() give
  a defensively-clamped count (min of the header field's own value and what actually fits in the
  received length) - the ordinary ESF-MEAS-style defensive pattern, not the ESF-RAW no-field-at-all
  fallback, since Version 0x01 DOES have a real count field.

  setRXMPMPmessageCallbackPtr() (the old v3 "push the whole message, including sync/checksum bytes,
  to another device" API) is NOT reimplemented, per the user's explicit instruction: the generic
  raw-frame relay mechanism added in Phase 30 (getUbxMessageRawLengthCallback()/
  getUbxMessageRawPtrCallback(), automatic for ANY message with a callback registered) already
  covers exactly that use case, for every message, not just PMP.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_RXM_PMP_MAX_LEN / UBX_RXM_PMP_MAX_USER_DATA

class ubxRXMPMP : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_RXM;
    const uint8_t ID = UBX_RXM_PMP;

    const char classStr[4] = "RXM";
    const char idStr[4] = "PMP";

    const uint8_t supportedVersions = 1; // Only Version 0x01 is modelled - see the file header comment above

    const uint16_t messageLength = UBX_RXM_PMP_MAX_LEN; // Maximum payload length (header + all userData bytes) - see above
    const uint8_t numCallbackCopies = 1;                 // Ordinary single-slot "latest wins" message - not a burst message like RXM-SFRBX/ESF-MEAS

    // Header field table - the 20-byte common header plus Version 0x01's fecBits/ebno, which
    // immediately follow it (see the file header comment above for why fecBits/ebno's byte offsets
    // are version-specific, and why only Version 0x01's placement is modelled). Read with the
    // ordinary getUbxMessageField()/getUbxMessageFieldCallback().
    static const uint8_t numFields = 10;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_RXM_PMP_I2C, UBLOX_CFG_MSGOUT_UBX_RXM_PMP_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_RXM_PMP_UART1, UBLOX_CFG_MSGOUT_UBX_RXM_PMP_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},            // Message version (0x00 / 0x01) - only 0x01 is correctly parsed by this class, see the file header comment above
        // reserved0 (byte 1) is not exposed
        {"numBytesUserData", ubxDataType8bit(UBX_CFG_U2), 2, -1, -1},   // Version 0x01: number of bytes of userData in this frame (0..504). Version 0x00: reserved (do not trust). Also used as blockCountField below
        {"timeTag", ubxDataType8bit(UBX_CFG_U4), 4, -1, -1},            // Time since startup when the frame started (ms)
        {"uniqueWord0", ubxDataType8bit(UBX_CFG_U4), 8, -1, -1},        // Received unique word, first half
        {"uniqueWord1", ubxDataType8bit(UBX_CFG_U4), 12, -1, -1},       // Received unique word, second half
        {"serviceIdentifier", ubxDataType8bit(UBX_CFG_U2), 16, -1, -1}, // Received service identifier
        {"spare", ubxDataType8bit(UBX_CFG_U1), 18, -1, -1},             // Received spare data
        {"uniqueWordBitErrors", ubxDataType8bit(UBX_CFG_U1), 19, -1, -1}, // Number of bit errors in both unique words
        {"fecBits", ubxDataType8bit(UBX_CFG_U2), 20, -1, -1},           // Number of bits corrected by FEC - Version 0x01 offset only, see the file header comment above
        {"ebno", ubxDataType8bit(UBX_CFG_U1), 22, -1, -1}};             // Energy per bit to noise power spectral density ratio (2^-3 dB) - Version 0x01 offset only
        // reserved1 (byte 23) is not exposed

    // Block field table - userData is opaque payload, not a struct of named fields, so it is
    // modelled as 0..UBX_RXM_PMP_MAX_USER_DATA repeated single-byte "blocks" (blockLength = 1), not
    // a header/block/footer record layout - see the file header comment above. Read with
    // getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing a block index up to (but
    // not including) getUbxMessageBlockCount()/...Callback()'s defensively-clamped count.
    static const uint8_t numBlockFields = 1;
    static const uint16_t blockHeaderLength = 24; // Bytes before userData[0] - Version 0x01 layout only
    static const uint16_t blockLength = 1;        // One byte per userData "block"

    const ubxField ubxBlockFields[numBlockFields] = {
        {"userData", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1}}; // One raw userData byte

    /**
     * @brief Construct a new ubxRXMPMP object and register it with the UBX message registry.
     *
     * UBX-RXM-PMP: PMP raw data (NEO-D9S). Registers this message's Class/ID, field table, callback-copy count and
     * per-port output-enable keys with the base ubxMessage class via addClassID(), making it
     * discoverable by the generic UBX dispatch, field-lookup, polling and auto-message
     * machinery in DevUBLOXGNSS.
     */
    ubxRXMPMP(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_RXM_PMP_MAX_USER_DATA,
                   "numBytesUserData"); // blockCountField - defensively cross-checked against actual length, see ubxMessage::getBlockCount()
    }
};

ubxRegisterMessage(ubxRXMPMP);
