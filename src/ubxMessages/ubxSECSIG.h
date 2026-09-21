/*
  ubxSECSIG.h

  v4 scaffolding: UBX-SEC-SIG - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Like NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX/
  RXM-SFRBX/MON-COMMS (see ubxNAVSAT.h etc.), SEC-SIG's payload is variable length - but only for
  message Version 3: a fixed 4-byte header (version, sigSecFlags, reserved0, jamNumCentFreqs)
  followed by 0..UBX_SEC_SEG_MAX_CENT_FREQ_VERSION3 identically-shaped 4-byte per-frequency blocks
  - see u-blox_structs.h's UBX_SEC_SIG_data_t.versions.version2 (Version 3 reuses this same
  layout unchanged) and AGENTS.md "Adding the variable-length UBX messages".

  Originally written against Version 2 (0x02), per the field layout documented at the time.
  Hardware-validated against a real ZED-X20P (CallbackExample9_SECSIG) and found that it actually
  outputs Version 3 (0x03), not Version 2 - observed sending 7 center frequencies with no jamming
  present. Version 3's byte layout, per the u-blox X20-HPG-2.11 interface description
  (UBXDOC-304424225-21617), is identical to Version 2's - the same 4-byte header + repeated
  4-byte center-frequency block shape - so this class's field tables needed no change, only
  supportedVersions/messageLength/the block-count constant below being repointed at Version 3.
  UBX_SEC_SEG_MAX_CENT_FREQ_VERSION3 (u-blox_structs.h) is set to 10, a headroom figure above the
  7 actually observed, not a documented hard maximum - u-blox hasn't published one for Version 3
  as far as this engagement has found.

  Only UBX-SEC-SIG Version 3 is supported by this class. Version 1 (a different, fixed 12-byte
  layout - see UBX_SEC_SIG_data_t.versions.version1 in u-blox_structs.h) is NOT modelled here at
  all: this class's field tables and byte offsets only make sense for a Version 2/3-shaped
  payload. A receiver that ever emits Version 1 SEC-SIG would be misparsed by this class (byte 3,
  read here as jamNumCentFreqs, falls inside Version 1's reserved1[3] and could drive a bogus
  block count) - not guarded against, since no per-message version dispatch mechanism exists in
  the registry (see AGENTS.md "Message versioning" - `ubxMessageVersion` is defined but not yet
  wired up anywhere). A real Version 2 message (if one is ever seen - the ZED-X20P has only been
  observed sending Version 3) would presumably still parse correctly too, given the identical
  documented layout, but that hasn't been hardware-tested.

  'ubxFields' below describes the header only, read with the ordinary
  getUbxMessageField()/getUbxMessageFieldCallback(). 'ubxBlockFields' describes one repeated
  4-byte center-frequency block, with offsets relative to the START OF THE BLOCK (not the
  message) - read with getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing the
  block index (0..jamNumCentFreqs-1) returned by the "jamNumCentFreqs" header field above.

  numCallbackCopies is 1 - like MON-COMMS, SEC-SIG is an ordinary periodic status message, not a
  message that arrives in back-to-back bursts, so the single-slot "latest wins" behavior used by
  every other registered message (other than RXM-SFRBX) is the correct choice here too.

  messageLength is set to UBX_SEC_SIG_MAX_LEN_VERSION3 - the MAXIMUM possible payload given the
  UBX_SEC_SEG_MAX_CENT_FREQ_VERSION3 headroom figure above, not a fixed wire size - exactly as for
  the other variable-length messages. storePayload() (in ubxMessageVector.h) already copies only
  the actual received length, clamped to it.

  centFreq is a 24-bit sub-field, wider than any bitfield previously registered in this codebase
  (every other bitfield sub-field fits within a single byte, so a U1/L-tagged union member alone
  is enough to read it back correctly). Reading it back needs the FULL 32-bit union member -
  see ubxAnyType::operator double() in ubxMessage.h - so centFreq is tagged X4/U4, not U1, even
  though its own bits are confined to 3 of the block's 4 bytes; extractFieldFrom() already always
  writes the extracted bits into value->U4 regardless of the declared tag, so this only affects
  which union member the generic double() conversion operator reads back from. Hardware-validated:
  CallbackExample9_SECSIG printed correct kHz values (e.g. 1575420, 1176450, 1227600, ...) for all
  7 observed center frequencies, confirming the 24-bit readback works correctly on real hardware.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_SEC_SIG_MAX_LEN_VERSION3 / UBX_SEC_SEG_MAX_CENT_FREQ_VERSION3

class ubxSECSIG : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_SEC;
    const uint8_t ID = UBX_SEC_SIG;

    const char classStr[4] = "SEC";
    const char idStr[4] = "SIG";

    const uint8_t supportedVersions = 3; // Version 3 only - see the file header comment above

    const uint16_t messageLength = UBX_SEC_SIG_MAX_LEN_VERSION3; // Maximum Version-3 payload length - see above
    const uint8_t numCallbackCopies = 1;

    static const uint8_t numFields = 7;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_SEC_SIG_I2C, UBLOX_CFG_MSGOUT_UBX_SEC_SIG_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_SEC_SIG_UART1, UBLOX_CFG_MSGOUT_UBX_SEC_SIG_UART2};

    const ubxField ubxFields[numFields] = {
        {"version", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1},
        {"sigSecFlags", ubxDataType8bit(UBX_CFG_X1), 1, -1, -1},
        {"jamDetEnabled", ubxDataType8bit(UBX_CFG_L), 1, 0, 1},
        {"jamState", ubxDataType8bit(UBX_CFG_U1), 1, 1, 2},
        {"spfDetEnabled", ubxDataType8bit(UBX_CFG_L), 1, 3, 1},
        {"spfState", ubxDataType8bit(UBX_CFG_U1), 1, 4, 3},
        {"jamNumCentFreqs", ubxDataType8bit(UBX_CFG_U1), 3, -1, -1}};

    static const uint8_t numBlockFields = 2;
    static const uint16_t blockHeaderLength = 4;
    static const uint16_t blockLength = 4;

    const ubxField ubxBlockFields[numBlockFields] = {
        {"centFreq", ubxDataType8bit(UBX_CFG_X4), 0, 0, 24}, // See the file header comment above re: tagging a 24-bit sub-field X4/U4, not U1
        {"jammed", ubxDataType8bit(UBX_CFG_L), 0, 24, 1}};

    ubxSECSIG(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_SEC_SEG_MAX_CENT_FREQ_VERSION3);
    }
};

ubxRegisterMessage(ubxSECSIG);
