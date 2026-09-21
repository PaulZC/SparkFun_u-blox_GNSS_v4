/*
  ubxRXMSFRBX.h

  v4 scaffolding: UBX-RXM-SFRBX - implemented as its own Class, per AGENTS.md
  "Implement each UBX message in its own Class". Like NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX (see
  ubxNAVSAT.h/ubxNAVSIG.h/ubxRXMRAWX.h/ubxRXMMEASX.h), SFRBX's payload is variable length: a fixed
  8-byte header (gnssId, svId, freqId, numWords, chn, version) followed by 0..UBX_RXM_SFRBX_MAX_WORDS
  identically-shaped 4-byte data words - see u-blox_structs.h's UBX_RXM_SFRBX_data_t and AGENTS.md
  "Adding the variable-length UBX messages". Same mechanism as NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX -
  nothing new needed designing for the variable-length shape itself.

  New territory for THIS message specifically: several SFRBX messages can arrive back-to-back in a
  single checkUblox() call, with no bus delay between them - see AGENTS.md "Adding support for
  RXM-SFRBX". numCallbackCopies is 14 (not 1, like every other message migrated so far) so that
  _callbackStorage (ubxMessage.h) is a genuine ring buffer, able to hold a whole burst until
  DevUBLOXGNSS::checkCallbacks() drains it - see ubxMessageVector::storePayload() (the write side)
  and DevUBLOXGNSS::checkCallbacks() (the read side, FIFO drain) in u-blox_GNSS.cpp.

  'ubxFields' below describes the header only, read with the ordinary
  getUbxMessageField()/getUbxMessageFieldCallback(). 'ubxBlockFields' describes one repeated 4-byte
  data word, with its offset relative to the START OF THE BLOCK (not the message) - read with
  getUbxMessageBlockField()/getUbxMessageBlockFieldCallback(), passing the block index
  (0..numWords-1) returned by the "numWords" header field above. The word ('dwrd') is deliberately
  NOT decomposed into named sub-bitfields here - its bits mean different things depending on which
  GNSS constellation and subframe it is; decoding that belongs to whatever consumes the raw words
  (e.g. an ephemeris parser), not to this field table.

  messageLength is set to UBX_RXM_SFRBX_MAX_LEN - the MAXIMUM possible payload, not a fixed wire
  size - exactly as for NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX. storePayload() (in ubxMessageVector.h)
  already copies only the actual received length, clamped to it.
*/

#pragma once

#include "../ubxMessage.h"
#include "../u-blox_config_keys.h"
#include "../u-blox_Class_and_ID.h"
#include "../u-blox_structs.h" // For UBX_RXM_SFRBX_MAX_LEN / UBX_RXM_SFRBX_MAX_WORDS

class ubxRXMSFRBX : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_RXM;
    const uint8_t ID = UBX_RXM_SFRBX;

    const char classStr[4] = "RXM";
    const char idStr[6] = "SFRBX";

    const uint8_t supportedVersions = 1;

    const uint16_t messageLength = UBX_RXM_SFRBX_MAX_LEN; // Maximum payload length - see above
    // Multiple buffered slots, not 1 - SFRBX messages can arrive in a back-to-back burst within a single
    // checkUblox() call. See AGENTS.md "Adding support for RXM-SFRBX". X20P needs additional buffers.
    const uint8_t numCallbackCopies = UBX_RXM_SFRBX_CALLBACK_BUFFERS;

    // Header field table - the 8 bytes that precede the repeated data words. Read with the
    // ordinary getUbxMessageField() / getUbxMessageFieldCallback(), exactly like any other message.
    static const uint8_t numFields = 6;
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_RXM_SFRBX_I2C, UBLOX_CFG_MSGOUT_UBX_RXM_SFRBX_SPI,
                                     UBLOX_CFG_MSGOUT_UBX_RXM_SFRBX_UART1, UBLOX_CFG_MSGOUT_UBX_RXM_SFRBX_UART2};

    const ubxField ubxFields[numFields] = {
        {"gnssId", ubxDataType8bit(UBX_CFG_U1), 0, -1, -1}, // GNSS identifier
        {"svId", ubxDataType8bit(UBX_CFG_U1), 1, -1, -1},   // Satellite identifier
        // reserved1 at byte 2 is not exposed
        {"freqId", ubxDataType8bit(UBX_CFG_U1), 3, -1, -1},  // Only used for GLONASS: GLONASS frequency slot
        {"numWords", ubxDataType8bit(UBX_CFG_U1), 4, -1, -1}, // The number of data words contained in this message (0..16)
        {"chn", ubxDataType8bit(UBX_CFG_U1), 5, -1, -1},      // The tracking channel number the message was received on
        {"version", ubxDataType8bit(UBX_CFG_U1), 6, -1, -1}}; // Message version (0x02 for this version)
        // reserved2 at byte 7 is not exposed

    // Block field table - one entry, describing a SINGLE 4-byte data word. The offset is relative
    // to the start of the block, not the message - see UBX_RXM_SFRBX_data_t's dwrd[] array in
    // u-blox_structs.h. Read with getUbxMessageBlockField() / getUbxMessageBlockFieldCallback(),
    // passing the block index (0..numWords-1) returned by the "numWords" header field above.
    // Deliberately not decomposed further - see the file header comment above.
    static const uint8_t numBlockFields = 1;
    static const uint16_t blockHeaderLength = 8; // Bytes before the first data word
    static const uint16_t blockLength = 4;        // Bytes per data word

    const ubxField ubxBlockFields[numBlockFields] = {
        {"dwrd", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1}}; // Raw 32-bit data word - decode per GNSS/subframe downstream

    ubxRXMSFRBX(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (const void *)&ubxFields, (const uint32_t *)msgOutKeys,
                   (const void *)&ubxBlockFields, numBlockFields, blockHeaderLength, blockLength,
                   UBX_RXM_SFRBX_MAX_WORDS);
    }
};

ubxRegisterMessage(ubxRXMSFRBX);
