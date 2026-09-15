/*
  ubxMessage.h

  v4 scaffolding: the per-message base class, plus the small set of shared types it needs
  (ubxAnyType, and the ubxDataType8bit() reduction macro). See AGENTS.md "Reference Scaffolding"
  and "moduleQueried" for the design this implements.

  This corrects several issues noted in AGENTS.md's "Known bugs in the prototype" against the
  preliminary prototype referenced there:
    - _moduleQueried is a single bool per message (not a per-field bitmask / numQueriedWords) -
      see AGENTS.md "moduleQueried".
    - Objects of this class (and its subclasses) are meant to be held as pointers with a stable
      lifetime (see ubxMessageVector.h), not copied by value - so _fields always stays valid.
    - ubxDataType8bit() did not previously exist anywhere in this codebase (only in the
      prototype); it is defined here as it is needed by both ubxNAVPVT.h and ubxMessageVector.h.
*/

#pragma once

#include <Arduino.h>
#include <string.h>

// Reduces one of the UBX_CFG_* type-tag constants (defined in u-blox_config_keys.h, and already
// used to tag VALGET/VALSET config keys) down to an 8-bit value suitable for storing in
// ubxMessage::ubxField::ubxDataType. One type vocabulary serves both config values and message
// payload fields - see AGENTS.md "Field data types are reused from the config-key encoding".
#define ubxDataType8bit(ubxType) ((uint8_t)(((ubxType >> 20) | (ubxType >> 12)) & 0xFF))

// The generic typed-value carrier used to move a single field's value across the
// Class/ID/field-name generic boundary (see DevUBLOXGNSS::getUBXfield() in u-blox_GNSS.h/.cpp).
typedef struct
{
    uint8_t ubxDataType; // Which union member is valid, from ubxDataType8bit()
    union
    {
        bool L;
        uint8_t U1;
        uint16_t U2;
        uint32_t U4;
        uint64_t U8;
        int8_t I1;
        int16_t I2;
        int32_t I4;
        int64_t I8;
        float R4;
        double R8;
    };
} ubxAnyType;

class ubxMessage
{
public:
    // One entry per field in the message payload.
    typedef struct
    {
        const char fieldName[20];  // Fixed-size array - keeps each field table self-contained (no separately-lived string needed)
        const uint8_t ubxDataType; // 8-bit type tag, from ubxDataType8bit() - see above
        const uint8_t startByte;   // Byte offset into the message PAYLOAD (i.e. into this object's _storage)
        const int8_t startBit;     // Bit offset within startByte if this is a sub-field; -1 otherwise
        const int8_t bitWidth;     // Bit width if this is a sub-field; -1 otherwise
    } ubxField;

    // One entry per version of a message whose payload shape has changed across firmware/protocol
    // versions. Not yet used by any message - see AGENTS.md "Still undesigned even after this
    // prototype" ("Message versioning"). Reserved for future use.
    typedef struct
    {
        const uint8_t version;
        const uint8_t numFields;
        const ubxField &firstField;
    } ubxMessageVersion;

    ubxMessage(void) {}

    virtual ~ubxMessage(void)
    {
        if (_storage != nullptr)
            delete[] _storage;
    }

    // Does this object represent this Class/ID?
    bool amI(uint8_t Class, uint8_t ID) const
    {
        return (Class == _Class) && (ID == _ID);
    }

    // Lazily allocate _storage - only when the message is actually used. This is how "delete the
    // header, save the RAM" (AGENTS.md) is meant to work for a message that is never instantiated
    // at all - see ubxMessageVector.h.
    bool initStorage(void)
    {
        if (_storage == nullptr)
        {
            _storage = new uint8_t[_messageLength];
            if (_storage != nullptr)
                memset(_storage, 0, _messageLength);
        }
        return (_storage != nullptr);
    }

    uint32_t getMsgOutKey(uint8_t commType) const
    {
        return _msgOutKeys[commType];
    }

    // Called once, from the subclass's own constructor, to register its identity/metadata into
    // the base class.
    void addClassID(uint8_t Class, uint8_t ID, const char *classStr, const char *idStr,
                     uint16_t messageLength, uint8_t numCallbackCopies, uint8_t numFields,
                     const void *ubxFields, const uint32_t *msgOutKeys)
    {
        _Class = Class;
        _ID = ID;
        _classStr = classStr;
        _idStr = idStr;
        _messageLength = messageLength;
        _numCallbackCopies = numCallbackCopies;
        _numFields = numFields;
        _fields = ubxFields;
        _storage = nullptr; // Only allocated when needed - see initStorage()
        _moduleQueried = false;
        _callbackPtr = nullptr;
        _automatic = false;
        _implicitUpdate = true;
        _addToFileBuffer = false;
        memcpy(_msgOutKeys, msgOutKeys, sizeof(uint32_t) * 4);
    }

    uint8_t _Class = 0;
    uint8_t _ID = 0;
    const char *_classStr = nullptr;
    const char *_idStr = nullptr;
    uint16_t _messageLength = 0;    // The message PAYLOAD length in bytes (matches e.g. UBX_NAV_PVT_LEN)
    uint8_t _numCallbackCopies = 0; // Reserved for future callback-copy support - not yet wired up for any message
    uint8_t *_storage = nullptr;    // Raw payload storage - nullptr until initStorage() is called
    uint8_t _numFields = 0;
    // Has fresh data arrived since the last time it was reported? A single flag for the whole
    // message, not a per-field bitmask - see AGENTS.md "moduleQueried".
    bool _moduleQueried = false;
    const void *_fields = nullptr;             // Points at the subclass's own, permanently-lived `ubxFields[]` table
    void (*_callbackPtr)(uint8_t *) = nullptr; // Reserved for future use - not yet wired up for any message
    bool _automatic = false;                   // Is the module set to output this message periodically?
    bool _implicitUpdate = true;               // true: getUBX() itself parses new data; false: caller must call checkUblox() itself
    bool _addToFileBuffer = false;             // Reserved for future use - not yet wired up for any message
    uint32_t _msgOutKeys[4] = {0, 0, 0, 0};    // UBLOX_CFG_MSGOUT_* keys for I2C, SPI, UART1, UART2
};
