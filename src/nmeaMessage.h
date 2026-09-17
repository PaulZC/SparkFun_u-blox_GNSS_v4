/*
  nmeaMessage.h

  Support for the NMEA message class nmeaMessage


*/

#pragma once

#include <Arduino.h>
#include <string.h>

#include "u-blox_config_keys.h" // Needed for the UBLOX_CFG_MSGOUT_NMEA_ID keys

// NMEA message field formats
typedef enum
{
    nmeaDataTypeString, // An array of ASCII char
    nmeaDataTypeTime, // Time in the format HHMMSS. Could have a variable number of decimal places: HHMMSS.SSS
    nmeaDataTypeDDMM, // Degrees Minutes for Latitude: 2 digit degrees, 2 digit minutes. Could have a variable number of decimal places: DDMM.MMMMMMM
    nmeaDataTypeDDDMM, // Degrees Minutes for Longitude: 3 digit degrees, 2 digit minutes. Could have a variable number of decimal places: DDDMM.MMMMMMM
    nmeaDataTypeChar, // A single ASCII character: e.g. N S E W M
    nmeaDataTypeDigit, // A single ASCII digit: 0-9
    nmeaDataTypeNumeric, // A numeric: could be integer or floating point; could be negative
} nmeaFieldFormats_t;

class nmeaMessage; // Forward declaration - see nmeaCallbackDataCommon_t below

// What actually crosses into the user's callback function
typedef struct
{
    const char nmea[4]; // Message name - "GGA" etc. - null-terminated
    nmeaMessage *messagePtr; // Opaque - do not dereference directly. Use getNmeaMessagePtr() /
                            // getNmeaMessageField() to read it.
} nmeaCallbackDataCommon_t;

class nmeaMessage
{
public:
    // One entry per field in the message payload.
    typedef struct
    {
        const char fieldName[20];   // Fixed-size array - keeps each field table self-contained (no separately-lived string needed)
        const uint8_t nmeaDataType; // 8-bit type tag, from nmeaFieldFormats_t - see above
        const uint8_t fieldNumber;  // Field number, starting from the Message ID
    } nmeaField;

    nmeaMessage(void) {}

    virtual ~nmeaMessage(void)
    {
        if (_storage != nullptr)
            delete[] _storage;
        if (_callbackStorage != nullptr)
            delete[] _callbackStorage;
    }

    // Does this object represent this message?
    bool amI(const char *msgId) const
    {
        return (strcmp(_msgId, msgId) == 0);
    }

    // Lazily allocate _storage - only when the message is actually used. This is how "delete the
    // header, save the RAM" (AGENTS.md) is meant to work for a message that is never instantiated
    // at all - see nmeaMessageVector.h.
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

    // Look up one field of this message, by name, in the given byte buffer (either this object's
    // own _storage for a live/polled read, or its _callbackStorage for a callback read.
    // Everything is returned as String. It's just easier that way...
    bool extractFieldFrom(const uint8_t *buffer, const char *fieldName, String &value) const
    {
        if (buffer == nullptr)
            return false;

        const nmeaField *fields = (const nmeaField *)_fields;
        for (uint8_t i = 0; i < _numFields; i++)
        {
            if (strncmp(fields[i].fieldName, fieldName, sizeof(fields[i].fieldName)) != 0)
                continue;

            switch (fields[i].nmeaDataType)
            {
            default:
                value = String("Unknown");
                break;
            case nmeaDataTypeString:
                // TODO
                break;
            case nmeaDataTypeTime:
                // TODO
                break;
            case nmeaDataTypeDDMM:
                // TODO
                break;
            case nmeaDataTypeDDDMM:
                // TODO
                break;
            case nmeaDataTypeChar:
                // TODO
                break;
            case nmeaDataTypeDigit:
                // TODO
                break;
            case nmeaDataTypeNumeric:
                // TODO
                break;
            }
        }
        return false; // Field name not found
    }

    // Called once, from the subclass's own constructor, to register its identity/metadata into
    // the base class.
    void addNMEA(const char *msgId, uint8_t messageLength, uint8_t numCallbackCopies, uint8_t numFields,
                const void *nmeaFields, const uint32_t *msgOutKeys)
    {
        _msgId = msgId;
        _messageLength = messageLength;
        _numCallbackCopies = numCallbackCopies;
        _numFields = numFields;
        _fields = nmeaFields;
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

    const char *_msgId = nullptr;
    uint8_t _messageLength = 0;    // The maximum message length in bytes. Messages could be less than this
    uint8_t _numCallbackCopies = 0; // Reserved for future callback-copy support - not yet wired up for any message
    uint8_t *_storage = nullptr;    // Raw payload storage - nullptr until initStorage() is called
    uint8_t _numFields = 0;
    // Has fresh data arrived since the last time it was reported? A single flag for the whole
    // message, not a per-field bitmask - see AGENTS.md "moduleQueried".
    bool _moduleQueried = false;
    const void *_fields = nullptr; // Points at the subclass's own, permanently-lived `nmeaFields[]` table
    uint8_t *_callbackStorage = nullptr; // Storage for the callback copy / copies - nullptr until initCallbackStorage() is called
    // Called by DevUBLOXGNSS::checkCallbacks() (via the generic registry walk) once fresh data is waiting
    void (*_callbackPtr)(nmeaMessage *) = nullptr;
    bool _callbackDataValid = false;           // Has storePayload() frozen a fresh copy into _callbackStorage that checkCallbacks() hasn't fired yet?
    bool _automatic = false;                   // Is the module set to output this message periodically?
    bool _implicitUpdate = true;               // true: getNMEA() itself parses new data; false: caller must call checkUblox() itself
    bool _addToFileBuffer = false;             // Set by logNMEA() using setAddToFileBuffer
    uint32_t _msgOutKeys[4] = {0, 0, 0, 0};    // UBLOX_CFG_MSGOUT_* keys for I2C, SPI, UART1, UART2
};

#include "nmeaMessageRegistry.h" // Message self-registration - see AGENTS.md "Message Class self-registration"

// Factory: extracts a named field from the message a callback just fired for, reading from its
// _callbackStorage (the copy storePayload() froze when the callback was queued) rather than its
// live _storage (which may already have been overwritten by newer data by the time the callback
// actually runs).
inline String getNmeaMessageFieldCallback(nmeaMessage *theMessage, const char *fieldName)
{
    String TODO;
    return TODO;
}
inline String getNmeaMessageField(nmeaMessage *theMessage, const char *fieldName)
{
    String TODO;
    return TODO;
}
