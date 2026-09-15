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

#include "u-blox_config_keys.h" // UBX_CFG_* type tags - needed by ubxAnyType::operator double() and ubxMessage::extractFieldFrom() below

// Reduces one of the UBX_CFG_* type-tag constants (defined in u-blox_config_keys.h, and already
// used to tag VALGET/VALSET config keys) down to an 8-bit value suitable for storing in
// ubxMessage::ubxField::ubxDataType. One type vocabulary serves both config values and message
// payload fields - see AGENTS.md "Field data types are reused from the config-key encoding".
#define ubxDataType8bit(ubxType) ((uint8_t)(((ubxType >> 20) | (ubxType >> 12)) & 0xFF))

// The generic typed-value carrier used to move a single field's value across the
// Class/ID/field-name generic boundary (see DevUBLOXGNSS::getUBXfield() in u-blox_GNSS.h/.cpp, and
// getFieldFromCallbackDataStruct() at the bottom of this file).
typedef struct ubxAnyType
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

    // A single, unambiguous implicit numeric conversion. See AGENTS.md "getCallbackDataStruct
    // Factory design pattern": "getFieldFromCallbackDataStruct will also need to use a Factory
    // method / design pattern to handle the different return types. If this is not possible,
    // identify the nearest alternative strategy which is possible." A non-template C++ function
    // cannot return a different static type per call depending on a runtime field-name string (the
    // field's type isn't known until the name is looked up at runtime) - and a template can't help
    // either, since there's no compile-time type to parameterize on. The nearest alternative is to
    // keep returning this single tagged-union type, but give it exactly one conversion operator, so
    // callers like `Serial.print(value)` or `value / 10` (see CallbackExample1_NAVHPPOSLLH.ino) get
    // one unambiguous conversion path regardless of which union member is actually valid. `double`
    // is the target because it exactly represents every integer type this library tags up to 32
    // bits, and to 2^53 for the rarer 64-bit fields - only a genuine 64-bit value outside that range
    // would lose precision, and none of the currently-registered messages have one. The trade-off:
    // Serial.print() will show a trailing ".00" for integer fields (double's default 2 decimal
    // places) rather than a clean integer - cosmetic, not a correctness issue.
    operator double() const
    {
        switch (ubxDataType)
        {
        case ubxDataType8bit(UBX_CFG_L):
            return L;
        case ubxDataType8bit(UBX_CFG_U1):
        case ubxDataType8bit(UBX_CFG_E1):
        case ubxDataType8bit(UBX_CFG_X1):
            return U1;
        case ubxDataType8bit(UBX_CFG_I1):
            return I1;
        case ubxDataType8bit(UBX_CFG_U2):
        case ubxDataType8bit(UBX_CFG_E2):
        case ubxDataType8bit(UBX_CFG_X2):
            return U2;
        case ubxDataType8bit(UBX_CFG_I2):
            return I2;
        case ubxDataType8bit(UBX_CFG_U4):
        case ubxDataType8bit(UBX_CFG_E4):
        case ubxDataType8bit(UBX_CFG_X4):
            return U4;
        case ubxDataType8bit(UBX_CFG_I4):
            return I4;
        case ubxDataType8bit(UBX_CFG_R4):
            return R4;
        case ubxDataType8bit(UBX_CFG_U8):
        case ubxDataType8bit(UBX_CFG_X8):
            return (double)U8;
        case ubxDataType8bit(UBX_CFG_I8):
            return (double)I8;
        case ubxDataType8bit(UBX_CFG_R8):
            return R8;
        default:
            return 0.0; // Unknown tag, or the "field not found" sentinel set by getFieldFromCallbackDataStruct()
        }
    }
} ubxAnyType;

class ubxMessage; // Forward declaration - see ubxCallbackDataCommon_t below

// What actually crosses into the user's callback function - see AGENTS.md "class ubxMessage needs
// separate callback storage" / "setAutoCallbackPtr" / "getCallbackDataStruct Factory design
// pattern". AGENTS.md envisaged this containing "an enum representing the type of data structure
// (UBX_NAV_PVT_data_t, UBX_NAV_HPPOSLLH_data_t, etc.)" - but the whole point of the v4 registry is
// that there is no longer a distinct C struct type per message, so there is nothing for such an
// enum to name. `messagePtr` carries the same information generically: it is the opaque handle
// getCallbackDataStruct()/getFieldFromCallbackDataStruct() use to navigate back to the message's
// own field table and callback storage, for any message, without a per-message enum to maintain.
typedef struct
{
    uint8_t Class; // Convenience - which message this is, without needing to dereference messagePtr
    uint8_t ID;
    ubxMessage *messagePtr; // Opaque - do not dereference directly. Use getCallbackDataStruct() /
                            // getFieldFromCallbackDataStruct() to read it.
} ubxCallbackDataCommon_t;

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
        if (_callbackStorage != nullptr)
            delete[] _callbackStorage;
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

    // Lazily allocate _callbackStorage - only once a callback is actually registered for this
    // message (see DevUBLOXGNSS::setAutoCallbackPtr()). Kept separate from _storage so a fast
    // producer (processUBXpacket, via storePayload()) can freeze a copy for the callback to read
    // without racing a consumer that hasn't got round to reading _storage yet - see AGENTS.md
    // "class ubxMessage needs separate callback storage". _numCallbackCopies is usually 1; only a
    // single copy is supported so far (every currently-registered message has numCallbackCopies ==
    // 1) - true ring-buffered multi-copy support for RXM-SFRBX/ESF-MEAS is still future work, per
    // AGENTS.md.
    bool initCallbackStorage(void)
    {
        if (_callbackStorage == nullptr)
        {
            _callbackStorage = new uint8_t[_messageLength * _numCallbackCopies];
            if (_callbackStorage != nullptr)
                memset(_callbackStorage, 0, _messageLength * _numCallbackCopies);
        }
        return (_callbackStorage != nullptr);
    }

    uint32_t getMsgOutKey(uint8_t commType) const
    {
        return _msgOutKeys[commType];
    }

    // Extract 'width' (1, 2, 4 or 8) little-endian bytes starting at byte offset 'offset' as an
    // unsigned value. Shared by extractFieldFrom() below (moved here from ubxMessageVector.h so it
    // can be used from either _storage or _callbackStorage, and by any ubxMessage, without a
    // circular ubxMessage.h <-> ubxMessageVector.h include).
    static uint64_t extractUnsignedBytes(const uint8_t *storage, uint16_t offset, uint8_t width)
    {
        uint64_t val = 0;
        for (uint8_t i = 0; i < width; i++)
            val |= ((uint64_t)storage[offset + i]) << (8 * i);
        return val;
    }

    // Extract 'bitWidth' bits (<= 32) starting at bit 'startBit' within the byte at 'offset'.
    static uint32_t extractBits(const uint8_t *storage, uint16_t offset, uint8_t startBit, uint8_t bitWidth)
    {
        uint32_t acc = 0;
        uint8_t bytesNeeded = (uint8_t)((startBit + bitWidth + 7) / 8);
        for (uint8_t i = 0; i < bytesNeeded; i++)
            acc |= ((uint32_t)storage[offset + i]) << (8 * i);
        uint32_t mask = (bitWidth >= 32) ? 0xFFFFFFFFu : (uint32_t)((1UL << bitWidth) - 1UL);
        return (acc >> startBit) & mask;
    }

    // Look up one field of this message, by name, in the given byte buffer (either this object's
    // own _storage for a live/polled read, or its _callbackStorage for a callback read - see
    // getFieldFromCallbackDataStruct() below). This is the shared core that used to be duplicated
    // between the live-read path and the (new) callback-read path; ubxMessageVector::extractValue()
    // delegates to this too.
    bool extractFieldFrom(const uint8_t *buffer, const char *fieldName, ubxAnyType *value) const
    {
        if (buffer == nullptr)
            return false;

        const ubxField *fields = (const ubxField *)_fields;
        for (uint8_t i = 0; i < _numFields; i++)
        {
            if (strncmp(fields[i].fieldName, fieldName, sizeof(fields[i].fieldName)) != 0)
                continue;

            value->ubxDataType = fields[i].ubxDataType;

            if (fields[i].startBit >= 0) // A sub-field: always extracted as an unsigned value
            {
                value->U4 = extractBits(buffer, fields[i].startByte, (uint8_t)fields[i].startBit, (uint8_t)fields[i].bitWidth);
                return true;
            }

            switch (fields[i].ubxDataType)
            {
            case ubxDataType8bit(UBX_CFG_L):
                value->L = (bool)buffer[fields[i].startByte];
                return true;
            case ubxDataType8bit(UBX_CFG_U1):
            case ubxDataType8bit(UBX_CFG_E1):
            case ubxDataType8bit(UBX_CFG_X1):
                value->U1 = (uint8_t)extractUnsignedBytes(buffer, fields[i].startByte, 1);
                return true;
            case ubxDataType8bit(UBX_CFG_I1):
                value->I1 = (int8_t)extractUnsignedBytes(buffer, fields[i].startByte, 1);
                return true;
            case ubxDataType8bit(UBX_CFG_U2):
            case ubxDataType8bit(UBX_CFG_E2):
            case ubxDataType8bit(UBX_CFG_X2):
                value->U2 = (uint16_t)extractUnsignedBytes(buffer, fields[i].startByte, 2);
                return true;
            case ubxDataType8bit(UBX_CFG_I2):
                value->I2 = (int16_t)extractUnsignedBytes(buffer, fields[i].startByte, 2);
                return true;
            case ubxDataType8bit(UBX_CFG_U4):
            case ubxDataType8bit(UBX_CFG_E4):
            case ubxDataType8bit(UBX_CFG_X4):
                value->U4 = (uint32_t)extractUnsignedBytes(buffer, fields[i].startByte, 4);
                return true;
            case ubxDataType8bit(UBX_CFG_I4):
                value->I4 = (int32_t)extractUnsignedBytes(buffer, fields[i].startByte, 4);
                return true;
            case ubxDataType8bit(UBX_CFG_R4):
            {
                uint32_t bits = (uint32_t)extractUnsignedBytes(buffer, fields[i].startByte, 4);
                memcpy(&value->R4, &bits, sizeof(float));
                return true;
            }
            case ubxDataType8bit(UBX_CFG_U8):
            case ubxDataType8bit(UBX_CFG_X8):
                value->U8 = extractUnsignedBytes(buffer, fields[i].startByte, 8);
                return true;
            case ubxDataType8bit(UBX_CFG_I8):
                value->I8 = (int64_t)extractUnsignedBytes(buffer, fields[i].startByte, 8);
                return true;
            case ubxDataType8bit(UBX_CFG_R8):
            {
                uint64_t bits = extractUnsignedBytes(buffer, fields[i].startByte, 8);
                memcpy(&value->R8, &bits, sizeof(double));
                return true;
            }
            default:
                return false; // Unknown ubxDataType
            }
        }
        return false; // Field name not found
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
        _callbackStorage = nullptr; // Only allocated when needed - see initCallbackStorage()
        _moduleQueried = false;
        _callbackPtr = nullptr;
        _callbackDataValid = false;
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
    const void *_fields = nullptr; // Points at the subclass's own, permanently-lived `ubxFields[]` table
    uint8_t *_callbackStorage = nullptr; // Storage for the callback copy / copies - nullptr until initCallbackStorage() is called
    // Called by DevUBLOXGNSS::checkCallbacks() (via the generic registry walk) once fresh data is
    // waiting - see AGENTS.md "setAutoCallbackPtr". Takes a ubxCallbackDataCommon_t*, not a raw
    // uint8_t* - see AGENTS.md "class ubxMessage needs separate callback storage".
    void (*_callbackPtr)(ubxCallbackDataCommon_t *) = nullptr;
    bool _callbackDataValid = false;           // Has storePayload() frozen a fresh copy into _callbackStorage that checkCallbacks() hasn't fired yet?
    bool _automatic = false;                   // Is the module set to output this message periodically?
    bool _implicitUpdate = true;               // true: getUBX() itself parses new data; false: caller must call checkUblox() itself
    bool _addToFileBuffer = false;             // Reserved for future use - not yet wired up for any message
    uint32_t _msgOutKeys[4] = {0, 0, 0, 0};    // UBLOX_CFG_MSGOUT_* keys for I2C, SPI, UART1, UART2
};

// Factory: hands back the opaque per-message object a callback's ubxCallbackDataCommon_t* points
// at, so getFieldFromCallbackDataStruct() can navigate its field table and extract a named field's
// value - see AGENTS.md "getCallbackDataStruct Factory design pattern". This is a free function
// (not a DevUBLOXGNSS method) because the callback itself is a free function with no `this` to
// call through - see CallbackExample1_NAVHPPOSLLH.ino. It doesn't need to consult any particular
// DevUBLOXGNSS instance's registry: ubxCallbackDataCommon_t already carries the messagePtr directly
// (set by DevUBLOXGNSS::checkCallbacks() when it fires the callback), so there is nothing left for
// a more elaborate factory to do.
inline ubxMessage *getCallbackDataStruct(ubxCallbackDataCommon_t *theData)
{
    if (theData == nullptr)
        return nullptr;
    return theData->messagePtr;
}

// Factory: extracts a named field from the message a callback just fired for, reading from its
// _callbackStorage (the copy storePayload() froze when the callback was queued) rather than its
// live _storage (which may already have been overwritten by newer data by the time the callback
// actually runs). See AGENTS.md "getFieldFromCallbackDataStruct will also need to use a Factory
// method / design pattern to handle the different return types. If this is not possible, identify
// the nearest alternative strategy which is possible" - see ubxAnyType::operator double() above for
// why this returns ubxAnyType rather than a genuinely per-field C++ type.
inline ubxAnyType getFieldFromCallbackDataStruct(ubxMessage *theMessage, const char *fieldName)
{
    ubxAnyType value;
    value.ubxDataType = 0xFF; // Sentinel - ubxDataType8bit() can never produce this value; operator double() returns 0.0 for it
    value.U8 = 0;
    if (theMessage != nullptr)
        theMessage->extractFieldFrom(theMessage->_callbackStorage, fieldName, &value);
    return value;
}
