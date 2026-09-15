## Background

This is an Arduino Library, written by SparkFun, to communicate with u\-blox GNSS modules.
It supports communication over I2C, UART (Serial), and SPI.
The top level class instances are: `SFE_UBLOX_GNSS` for I2C, `SFE_UBLOX_GNSS_SERIAL` for UART, and `SFE_UBLOX_GNSS_SPI` for SPI.
The hardware physical interface is defined in sfe\_bus.h and sfe\_bus.cpp.

SparkFun\_u\-blox\_GNSS\_v4 is to be an updated version of [SparkFun\_u\-blox\_GNSS\_v3](https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3)

u\-blox GNSS modules use a binary communication protocol called "UBX".
They also support "NMEA" \- defined by the National Marine Electronics Association standard NMEA 0183: Standard For Interfacing Marine Electronic Devices.
They also support "RTCM" \- defined by the Radio Technical Commission for Maritime Services standard RTCM STANDARD 10403.3: Differential GNSS Services \- Version 3.

## Important References

[u\-blox HPG 2.11 Interface description](https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf)
[ZED\-X20P\-01B Data sheet](https://www.u-blox.com/sites/default/files/documents/ZED-X20P-01B_DataSheet_UBXDOC-304424225-21281.pdf)
[ZED\-X20P Integration manual](https://www.u-blox.com/sites/default/files/documents/ZED-X20P_IntegrationManual_UBXDOC-963802114-12901.pdf)

National Marine Electronics Association standard NMEA 0183 is a copyright document. A copy will be provided to you when you request it in a chat session.

Radio Technical Commission for Maritime Services standard RTCM STANDARD 10403.3 is a copyright document. A copy will be provided to you when you request it in a chat session.

## The Problem

The coding style in SparkFun\_u\-blox\_GNSS\_v3 needs improvement.
The code contains many repetitions of the same or similar code blocks for each UBX message.
In the [v3 Library notes for Adding\_New\_Messages](https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/Adding_New_Messages.md) you can see how the code to support each message is added to `end()`, `autoLookup()`, `processUBXpacket()`, `checkCallbacks()`, etc.
The key objective for v4 is to avoid this code repetition.

## The Objective

The objective is to re\-write the existing code from the v3 library, to create the new v4 library.

The top level class instances `SFE_UBLOX_GNSS`, `SFE_UBLOX_GNSS_SERIAL` and `SFE_UBLOX_GNSS_SPI` are to be retained.

The hardware physical interface defined in sfe\_bus.h and sfe\_bus.cpp is to be retained.

The u\-blox UBX Class and ID definitions in u\-blox\_Class\_and\_ID.h are to be retained.

The examples in the examples folder are to be retained and not modified.

## Implement each UBX message in its own Class

For each UBX message supported by the original v3 library, implement support for that message in a separate Class.

E.g.: for UBX\-NAV\-PVT, add a file called ubxNAVPVT.h containing:

```
// UBX-NAV-PVT (0x01 0x07): Navigation position velocity time solution
class ubxNAVPVT : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_PVT;

    const char classStr[4] = "NAV"; // The message Class - from the interface description
    const char idStr[4] = "PVT";    // The message ID. Increase the array size if needed

    // These are the configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_PVT_SPI,
                                    UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART2};

    const ubxField ubxFields[] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"year", ubxDataType8bit(UBX_CFG_U2), 4, -1, -1},
        {"month", ubxDataType8bit(UBX_CFG_U1), 6, -1, -1},
        {"day", ubxDataType8bit(UBX_CFG_U1), 7, -1, -1},
        {"hour", ubxDataType8bit(UBX_CFG_U1), 8, -1, -1},
        {"min", ubxDataType8bit(UBX_CFG_U1), 9, -1, -1},
        {"sec", ubxDataType8bit(UBX_CFG_U1), 10, -1, -1},
        {"valid", ubxDataType8bit(UBX_CFG_X1), 11, -1, -1},
        {"validDate", ubxDataType8bit(UBX_CFG_L), 11, 0, 1},
        {"validTime", ubxDataType8bit(UBX_CFG_L), 11, 1, 1},
        {"fullyResolved", ubxDataType8bit(UBX_CFG_L), 11, 2, 1},
        {"validMag", ubxDataType8bit(UBX_CFG_L), 11, 3, 1},
        {"tAcc", ubxDataType8bit(UBX_CFG_U4), 12, -1, -1},
        {"nano", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"fixType", ubxDataType8bit(UBX_CFG_U1), 20, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 21, -1, -1},
        {"gnssFixOK", ubxDataType8bit(UBX_CFG_L), 21, 0, 1},
        {"diffSoln", ubxDataType8bit(UBX_CFG_L), 21, 1, 1},
        {"psmState", ubxDataType8bit(UBX_CFG_U1), 21, 2, 3},
        {"headVehValid", ubxDataType8bit(UBX_CFG_L), 21, 5, 1},
        {"carrSoln", ubxDataType8bit(UBX_CFG_U1), 21, 6, 2},
        {"flags2", ubxDataType8bit(UBX_CFG_X1), 22, -1, -1},
        {"confirmedAvai", ubxDataType8bit(UBX_CFG_L), 22, 5, 1},
        {"confirmedDate", ubxDataType8bit(UBX_CFG_L), 22, 6, 1},
        {"confirmedTime", ubxDataType8bit(UBX_CFG_L), 22, 7, 1},
        {"numSV", ubxDataType8bit(UBX_CFG_U1), 23, -1, -1},
        {"lon", ubxDataType8bit(UBX_CFG_I4), 24, -1, -1},
        {"lat", ubxDataType8bit(UBX_CFG_I4), 28, -1, -1},
        {"height", ubxDataType8bit(UBX_CFG_I4), 32, -1, -1},
        {"hMSL", ubxDataType8bit(UBX_CFG_I4), 36, -1, -1},
        {"hAcc", ubxDataType8bit(UBX_CFG_U4), 40, -1, -1},
        {"vAcc", ubxDataType8bit(UBX_CFG_U4), 44, -1, -1},
        {"velN", ubxDataType8bit(UBX_CFG_I4), 48, -1, -1},
        {"velE", ubxDataType8bit(UBX_CFG_I4), 52, -1, -1},
        {"velD", ubxDataType8bit(UBX_CFG_I4), 56, -1, -1},
        {"gSpeed", ubxDataType8bit(UBX_CFG_I4), 60, -1, -1},
        {"headMot", ubxDataType8bit(UBX_CFG_I4), 64, -1, -1},
        {"sAcc", ubxDataType8bit(UBX_CFG_U4), 68, -1, -1},
        {"headAcc", ubxDataType8bit(UBX_CFG_U4), 72, -1, -1},
        {"pDOP", ubxDataType8bit(UBX_CFG_U2), 76, -1, -1},
        {"flags3", ubxDataType8bit(UBX_CFG_X2), 78, -1, -1},
        {"invalidLlh", ubxDataType8bit(UBX_CFG_L), 78, 0, 1},
        {"lastCorrectionAge", ubxDataType8bit(UBX_CFG_U1), 78, 1, 4},
        {"headVeh", ubxDataType8bit(UBX_CFG_I4), 84, -1, -1},
        {"magDec", ubxDataType8bit(UBX_CFG_I2), 88, -1, -1},
        {"magAcc", ubxDataType8bit(UBX_CFG_U2), 90, -1, -1}
    };
};
```

The `ubxField` is defined in `ubxMessage` and could be something like:

```
    // Definition of the data type to hold a single UBX message field
    typedef struct
    {
        const char *fieldName;  // The field name - taken from the u-blox interface description
        const uint8_t ubxDataType; // The UBX data type L,U1,U2 etc. in 8-bit form from the macro ubxDataType8bit
        const uint8_t startByte;   // The position of the field in the UBX message payload. (Add 6 when extracting)
        const int8_t startBit;     // The bit position if this is a bit field. -1 otherwise
        const int8_t bitWidth;     // The bit width if this is a bit field. -1 otherwise
    } ubxField;
```

`ubxNAVPVT` is to be a singleton, which is automatically instantiated on execution.

If the user wishes to save memory, they can delete ubxNAVPVT.h. The remainder of the code will compile, but will not provide support for NAV PVT.

`getPVT()`, `getLatitude()`, `getLongitude()` and `getAltitudeMSL()` are simple helper methods defined within the parent `class DevUBLOXGNSS`.
The code for `getLatitude()` could be:

```
    // Get the current latitude in degrees
    // Returns a long representing the number of degrees *10^-7
    int32_t DevUBLOXGNSS::getLatitude(uint16_t maxWait)
    {
        ubxAnyType *value;
        if (!getUBXfield(UBX_CLASS_NAV, UBX_NAV_PVT, "lat", ubxAnyType * value, uint16_t maxWait))
            return 0;
        return value.I4;
    }
```

By default, `getPVT()` will Poll the NAV\-PVT message. The code for `getPVT()` could be:

```
bool DevUBLOXGNSS::getPVT(uint16_t maxWait)
{
    return getUBX(UBX_CLASS_NAV, UBX_NAV_PVT, maxWait);
}
```

The `getUBX()` method is to be provided by the `class DevUBLOXGNSS`. It is a generic method which:

- Creates storage to hold the full UBX message
  - The storage is to be created using a `virtual` `alloc` method
- Understands if the message is Periodic (has had a message rate set), or whether it should be Polled (requested once)
- If the NAV\-PVT message needs to be Polled (requested)
  - The library should send a UBX message containing:
    - UBX Class `UBX_CLASS_NAV`
    - UBX ID `UBX_NAV_PVT`
    - Length zero
  - Wait for up to `maxWait` milliseconds for the message to arrive, calling `checkUbloxInternal()` every few 10s of millisconds
  - If the NAV\-PVT message is received within `maxWait`, it is stored in the allocated storage
  - `getPVT()` returns `true` if a NAV\-PVT message was received within `maxWait`, `false` otherwise
  - `getPVT()` is blocking in this case. It will wait / stall for the full `maxWait` if needed
- If the NAV\-PVT message is periodic:
  - `getUBX()` will call the method `checkUbloxInternal()` to check for the availablelity of new I2C/UART/SPI data
  - If a new NAV\-PVT message has arrived, it is stored in the allocated storage
  - `getPVT()` returns true if `checkUbloxInternal()` provided a new NAV\-PVT message, `false` otherwise.
  - `getPVT()` is non\-blocking in this case. After calling `checkUbloxInternal()` once, it returns `true` or `false` immediately. It does not wait for `maxWait` milliseconds.

## Reference Scaffolding (from a preliminary prototype)

A preliminary, human\-written, **incomplete** prototype of this v4 architecture exists at
`C:\Users\pc235\Documents\SparkX\SparkFun_u-blox_GNSS_v4`. It is not part of this repo and is not
wired into the build here — it is referenced purely as design input, because it makes concrete
several things the sketch above only implies. It also contains real bugs (listed below) that must
NOT be carried over verbatim. It also uses a per\-field `numQueriedWords`/`_queried` bitmask for tracking
whether each field has been read, which has since been superseded \- see "moduleQueried" below: use a single
`bool` per message instead. The scaffolding below has been updated to reflect that decision. The relevant
files in the prototype are:

- `src/ubxMessage.h` \- the per\-message base class
- `src/ubxNAVPVT.h` \- a concrete per\-message subclass (NAV\-PVT)
- `src/ubxMessageVector.h` \- the registry that owns all per\-message objects and does the generic (Class, ID) dispatch
- `src/u-blox_GNSS.h` / `.cpp` \- `DevUBLOXGNSS`, updated to hold and use the registry
- `src/u-blox_config_keys.h` \- confirms the field\-type tagging scheme (see below)

### `ubxMessage` base class

Every per\-message class derives from `ubxMessage`, which defines two nested types and holds all of
the per\-message runtime state generically (i.e. this state used to be a separate hand\-written set of
variables/arrays per message in v3; in v4 it lives once, in the base class):

```
class ubxMessage
{
public:
    // One entry per version of a message that has changed shape across firmware versions
    typedef struct
    {
        const uint8_t version;
        const uint8_t numFields;
        const ubxField &firstField;
    } ubxMessageVersion;

    // One entry per field in the message payload
    typedef struct
    {
        const char fieldName[20];  // Fixed-size array, not a pointer - keeps each field table self-contained
        const uint8_t ubxDataType; // 8-bit type tag - see "Field data types" below
        const uint8_t startByte;   // Byte offset into the payload
        const int8_t startBit;     // Bit offset if this is a sub-field of a byte; -1 otherwise
        const int8_t bitWidth;     // Bit width if this is a sub-field; -1 otherwise
    } ubxField;

    bool amI(uint8_t Class, uint8_t ID);         // Does this object represent this Class/ID?
    bool initStorage(void);                       // Lazily allocate _storage (only when the message is actually used)
    uint32_t getMsgOutKey(uint8_t commType);       // commType: I2C/SPI/UART1/UART2 index into _msgOutKeys

    // Called once, from the subclass's own constructor, to register its identity/metadata into the base class
    void addClassID(uint8_t Class, uint8_t ID, const char *classStr, const char *idStr,
                     uint16_t messageLength, uint8_t numCallbackCopies, uint8_t numFields,
                     void *ubxFields, uint32_t *msgOutKeys);

    uint8_t _Class, _ID;
    const char *_classStr, *_idStr;
    uint16_t _messageLength;      // Full wire length incl. header/class/id/length/checksum
    uint8_t _numCallbackCopies;   // How many ring-buffer copies to keep for the callback path
    uint8_t _head, _tail;         // Ring buffer indices for callback copies
    uint8_t *_storage;            // Raw payload storage - nullptr until initStorage() is called
    uint8_t _numFields;
    bool _moduleQueried;          // Has the message data been read since it last arrived? One flag for the whole message, not a per-field bitmask - see "moduleQueried" below
    void *_fields;                // Points at the subclass's own `ubxFields[]` table
    void (*_callbackPtr)(uint8_t *);
    bool _automatic;              // Is the module set to output this message periodically?
    bool _implicitUpdate;         // true: getUBX() itself parses new data; false: caller must call checkUblox() itself
    bool _addToFileBuffer;
    uint32_t _msgOutKeys[4];      // UBLOX_CFG_MSGOUT_* keys for I2C, SPI, UART1, UART2
};
```

Notably, `initStorage()` allocates `_storage` on first use, sized `_messageLength * (_numCallbackCopies + 1)`

- this is how "delete the header, save the RAM" is meant to work: if a message class is never
  instantiated, nothing is ever allocated for it.

### Per\-message subclass pattern

A concrete message (e.g. `ubxNAVPVT.h`) declares all of its own metadata as `const` members (Class, ID,
name strings, message length, the four `msgOutKeys`, and its `ubxFields[]` table), and its constructor's
only job is to hand all of that to the base class via `addClassID(...)`\:

```
class ubxNAVPVT : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_PVT;
    const char classStr[4] = "NAV";
    const char idStr[4] = "PVT";
    const uint8_t supportedVersions = 1;              // For future use with ubxMessageVersion
    const uint16_t messageLength = 100;
    const uint8_t numCallbackCopies = 1;
    static const uint8_t numFields = 46;
    const uint32_t msgOutKeys[4] = { UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_PVT_SPI,
                                      UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART2 };
    const ubxField ubxFields[numFields] = { /* {fieldName, type, startByte, startBit, bitWidth}, ... */ };

    ubxNAVPVT(void)
    {
        addClassID(Class, ID, classStr, idStr, messageLength, numCallbackCopies,
                   numFields, (void *)&ubxFields, (uint32_t *)msgOutKeys);
    }
};
```

This confirms the field\-table shape sketched earlier in this document is on the right track; the one
refinement is that `fieldName` should be a fixed `char[20]` rather than a `const char *`, so each
`ubxField` entry is fully self\-contained (no separately\-lived string needed).

### Field data types are reused from the config\-key encoding, not reinvented

`u-blox_config_keys.h` already tags every `UBLOX_CFG_*` VALGET/VALSET key with a type, packed into the
key's upper bits, using constants like:

```
const uint32_t UBX_CFG_L  = 0x01001000; // bool
const uint32_t UBX_CFG_U1 = 0x01002000; // uint8_t
...
const uint32_t UBX_CFG_R4 = 0x0100A000; // float
const uint32_t UBX_CFG_R8 = 0x0100B000; // double
const uint32_t UBX_CFG_SIZE_MASK = 0x0F00F000;

#define ubxDataType8bit(ubxType) ((uint8_t)(((ubxType >> 20) | (ubxType >> 12)) & 0xFF))
```

The full set is `L, U1/E1/X1, I1, U2/E2/X2, I2, U4/E4/X4, I4, R4, U8/X8, I8, R8` (signed/unsigned/enum/bitfield
variants of 1/2/4/8\-byte values, plus 4\- and 8\-byte floats). `ubxMessage::ubxField.ubxDataType` reuses this
exact same tag (reduced to 8 bits by `ubxDataType8bit()`) rather than inventing a separate type system for
message fields \- one type vocabulary serves both config values and message payload fields.

### `ubxAnyType` \- the generic typed\-value carrier

This is how a field's value crosses the Class/ID/field\-name generic boundary without needing per\-field
generated code:

```
typedef struct
{
    uint8_t ubxDataType; // Which union member is valid, from ubxDataType8bit()
    union
    {
        bool L;
        uint8_t U1; uint16_t U2; uint32_t U4; uint64_t U8;   // also E1/X1, E2/X2, E4/X4, X8
        int8_t I1;   int16_t I2;   int32_t I4;   int64_t I8;
        float R4;   double R8;
    };
} ubxAnyType;

bool getUBXfield(uint8_t Class, uint8_t ID, const char *field, ubxAnyType *value, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
```

Per\-field convenience getters like `getLatitude()` are thin wrappers that call `getUBXfield(..., "lat", &v, ...)`
and return `v.I4`.

### `ubxMessageVector` \- the registry and the generic (Class, ID) dispatch

`DevUBLOXGNSS` holds one member, `ubxMessageVector ubxMessages;`, which owns every compiled\-in per\-message
object and is the single place that replaces what used to be a repeated switch/if\-chain per message in v3.
Its constructor is where each supported message gets instantiated (e.g. `ubxNAVPVT UBXNAVPVT;` then added to
the collection) \- this is the "self\-registration" step, and it is also the natural place a future
build\-time/conditional\-compilation mechanism would need to hook in for the "delete the header, it just won't
be registered" behaviour described earlier (see Known Gaps below \- the prototype does not yet make this
conditional).

Once populated, the registry exposes generic, Class/ID\-keyed operations that `DevUBLOXGNSS` calls instead of
per\-message code: `initStorage`, `isAutomatic` / `setAutomatic`, `implicitUpdate` / `setImplicitUpdate`,
`moduleQueried` / `setModuleQueried` (a single `bool` per message \- has the message data been read since it
last arrived; not a per\-field bitmask \- see "moduleQueried" below), `addToFileBuffer` / `setAddToFileBuffer`, `getMsgOutKey`, `setCallback`,
`getClassAndID` (string name \-\> Class/ID lookup), and `extractValue` (field name \-\> typed value, backing
`getUBXfield`). Each simply does a linear search over the registered messages matching `amI(Class, ID)`, then
acts on that one object's state \- this is the mechanism that eliminates per\-message repetition in the parts
of `DevUBLOXGNSS` that have been migrated to use it (see below).

The registry also centralizes the raw byte/bit extraction helpers previously duplicated throughout v3:
`extractU8/I8/Long/SignedLong/Int/SignedInt/Byte/SignedChar/Float/Double`, plus bit\-level
`extractUnsignedBits` / `extractSignedBits` for the `startBit`/`bitWidth` sub\-fields, using small
`union { unsigned; signed/float; }` converters to reinterpret raw bytes safely.

### How this plugs into `DevUBLOXGNSS`

`getUBX(Class, ID, maxWait)` demonstrates the intended poll\-vs\-automatic decision logic named in "The
Objective" above, now driven generically through the registry instead of per\-message code:

1. `ubxMessages.initStorage(Class, ID)` \- ensure storage exists
2. Look up `isAutomatic` and `implicitUpdate` for this Class/ID via the registry
3. If automatic \+ implicit: call `checkUbloxInternal()` once (non\-blocking) and return whether fresh data arrived
4. If automatic \+ explicit: return false (caller is expected to call `checkUblox()` themselves)
5. Otherwise: build and send a zero\-length poll packet for `Class`/`ID` and block up to `maxWait`

Step 3 also has to *clear* the flag once it has reported fresh data. `moduleQueried` now represents
freshness for the whole message as a single `bool` (see "moduleQueried" below), and \- unlike the old
per\-field bitmask \- no field\-level getter (e.g. `getLatitude()`) clears anything on read any more. So
immediately after `ubxMessages.moduleQueried(Class, ID, &queried)` returns `true`, `getUBX()` must call
the equivalent of `setModuleQueried(Class, ID, false)` before returning. Without that, the periodic case
would keep reporting the same old data as "fresh" on every subsequent call, and "returns true if a new
message has arrived *since the last call*" (see "moduleQueried" below) would not actually hold.

`setAutoUBXrate`, `assumeAutoUBX`, `flushUBX`, and `logUBX` are fully migrated to this pattern already (each
is now a short, generic function that goes through `ubxMessages.*` instead of a per\-message implementation)
and are good models to copy from directly.

### Known bugs in the prototype \- do NOT copy these as\-is

- **Object slicing / dangling field pointer.** The registry stores `std::vector<ubxMessage>` (by value).
  Pushing a derived object (`ubxNAVPVT`) into a base\-typed vector slices it down to the base class, and
  worse, the base's `_fields` pointer is set (via `addClassID`) to point at the *derived* object's
  `ubxFields[]` array \- which in the prototype's constructor is a local stack variable that goes out of
  scope immediately after the `push_back`. The real implementation needs the registry to store pointers
  (e.g. `std::vector<ubxMessage *>`, or owned instances of the concrete subclasses) so `_fields` stays valid
  for the program's lifetime.
- **`getUBX()`'s poll path is an unfinished copy\-paste.** In the prototype it still hardcodes
  `packetCfg.cls = UBX_CLASS_NAV; packetCfg.id = UBX_NAV_POSECEF;` regardless of the `Class`/`ID` arguments
  actually passed in \- a leftover from the old per\-message v3 function it was adapted from. The real version
  must build the poll packet from the function's own `Class`/`ID` parameters.
- **`getUBX()` never clears the freshness flag after reporting it.** In the automatic\+implicit branch, the
  prototype reads `moduleQueried` and returns it, but never clears it afterwards. Under the old per\-field
  bitmask this was masked because each field getter (e.g. `getLatitude()`) cleared its own bit on read; with
  the single `bool _moduleQueried` per message decided in "moduleQueried" below \- and field getters no
  longer clearing anything \- `getUBX()`/`getPVT()` itself must clear the flag immediately after reporting
  `true`, or the periodic case would report stale data as fresh on every subsequent call.
- **`getUBXfield()` doesn't return a value and mismatches its own signature** \- it calls
  `ubxMessages.extractValue(Class, ID, field, *value)` (dereferencing `value` instead of passing the pointer)
  and doesn't `return` the result.
- **`ubxMessageVector::extractValue()` is unfinished** \- it has unclosed loops and treats `value` (a pointer)
  as if it were an object (`value.ubxDataType = ...`). It needs a proper implementation: locate the field by
  name in `_fields`, then use `startBit`/`bitWidth` (via `extractUnsignedBits`/`extractSignedBits`) or a plain
  byte extraction, keyed on `ubxDataType`, to fill in the matching `ubxAnyType` union member.
- **The receive/parse side was not migrated.** `autoLookup()` (and, by implication, `checkUbloxInternal()` /
  `processUBXpacket()`) in the prototype still use the old v3\-style dedicated per\-message pointers
  (`packetUBXNAVPOSECEF`, `packetUBXNAVSTATUS`, `packetUBXNAVPVT`, ...) and a per\-Class/ID switch statement,
  instead of querying `ubxMessages`. Only the *configuration/control\-plane* half of `DevUBLOXGNSS`
  (`setAutoUBXrate`, `flushUBX`, `logUBX`, etc.) was actually migrated to the generic registry in this
  prototype \- the *receive\-and\-store\-incoming\-data* half still needs the same treatment to fully deliver on
  "The Problem" above. This is the largest remaining piece of design work.
- **An earlier, abandoned draft exists alongside the current one.** `u-blox_GNSS__.h`/`.cpp` (double
  underscore) is an older, superseded attempt with a lowercase\-`d` `devUBLOXGNSS` class and a different API
  shape \- `getUBX(Class, ID, field, ...)` returning the typed value directly as `int32_t`, plus separate
  `isPeriodic()`/`periodic()` methods. It predates the `ubxAnyType`\-based design above and should be ignored;
  it's noted here only so that API shape isn't accidentally reintroduced as if it were new.
- **Naming.** The prototype's top\-level bus wrapper class is called `sfeUBLOXGNSS`, and it derives from
  `DevUBLOXGNSS`. This repo's "Objective" section above requires the top\-level class *instances* to remain
  `SFE_UBLOX_GNSS`, `SFE_UBLOX_GNSS_SERIAL`, and `SFE_UBLOX_GNSS_SPI` \- keep those retained names; treat the
  prototype's single combined class name as an implementation detail to rename/restructure, not to copy
  verbatim.

### Still undesigned even after this prototype

- **Conditional registration.** "Delete `ubxNAVPVT.h` and the rest still compiles, just without NAV\-PVT
  support" implies the registry's constructor can't unconditionally list every message class by name (as the
  prototype does) \- some mechanism (e.g. each message header guarding its own registration, or a
  generated/conditional list) is still needed so a missing header doesn't just fail to compile.
- **Message versioning.** `ubxMessage::ubxMessageVersion` exists in the base class but is not yet used by
  `ubxNAVPVT` (`supportedVersions = 1`) or referenced anywhere in the registry \- the mechanism for messages
  whose payload shape has changed across firmware/protocol versions still needs to be designed.

## moduleQueried

The original v3 code uses a bit field called `moduleQueried` to indicate if a field has been queried (read by the user).

For NAV\-PVT, the `UBX_NAV_PVT_t` type contains `UBX_NAV_PVT_moduleQueried_t moduleQueried;`.
`moduleQueried` contains two 32\-bit unions `moduleQueried1` and `moduleQueried2`.
Each `union` contains: `all` which allows all bits to be accessed in a single read or write; and `bits` which are the individual bit field.
The `moduleQueried1` `bits` `lat` indicates if the GNSS latitude `int32_t lat` in `UBX_NAV_PVT_data_t data` has been queried.

`moduleQueried` indicates if the data has been queried / read. It indicates if the data is "fresh" \- has not been read before.
If the NAV\-PVT message is not periodic ("automatic") and is being Polled, if the GNSS latitude is available and has not been read before, then `getLatitude()` clears the `moduleQueried.moduleQueried1.bits.lat` bit and returns the `data.lat`. If the user calls `getLatitude()` again, `moduleQueried.moduleQueried1.bits.lat` is clear and so the NAV\-PVT message is Polled again, to ensure the user receives fresh data.

`moduleQueried` is a level of complexity we do not need. For v4 of the library, the individual `moduleQueried` bit fields for each field (e.g. `lat`) should be removed. A single `bool` flag for each message should be retained, indicating if the message data as a whole is fresh or stale. Some messages already use this single `bool moduleQueried`\: NAV\-SAT (`UBX_NAV_SAT_t`) and NAV\-SIG (`UBX_NAV_SIG_t`) are two examples. The same approach should be applied to all messages.

The behaviour should be as follows:

- If the message is not Periodic ("Automatic") and is being Polled each time, `getPVT()` should Poll (request) the NAV\-PVT message and return `true` if it is received within the `maxWait` timeout. `getLatitude()` will return the most recent `data.lat`, even if it is "stale" (has been read before).
- If the message is Periodic, `getPVT()` returns `true` if a new NAV\-PVT message has arrived since the last call. `getLatitude()` will return the most recent `data.lat`, even if it is "stale" (has been read before).

The code shared from `C:\Users\pc235\Documents\SparkX\SparkFun_u-blox_GNSS_v4` includes `numQueriedWords`. E.g. `const uint8_t numQueriedWords = 2;   // We need this many words (uint32_t) to hold the queried flags`. Again, do not use this approach. Use a single `bool` to indicate if the whole message is fresh or stale.

## Callbacks

The original v3 code included support for callbacks. The arrival of any Periodic messages could be set to trigger a callback.

Using NAV-PVT as an example:

* Calling `myGNSS.setAutoPVTcallbackPtr(&printPVTdata);` in `setup` would:
    * Instruct the GNSS to output the NAV-PVT message periodically (performed by `setAutoPVT()`)
    * Register a callback for the NAV-PVT message (by setting `packetUBXNAVPVT->callbackPointerPtr` to the callback reference)
* The callback would be defined as:

```
// Callback: printPVTdata will be called when new NAV PVT data arrives
// See u-blox_structs.h for the full definition of UBX_NAV_PVT_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoPVTcallback
//        /                  _____  This _must_ be UBX_NAV_PVT_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printPVTdata(UBX_NAV_PVT_data_t *ubxDataStruct)
```

Using a different type for the parameter for each callback (`UBX_NAV_PVT_data_t`, `UBX_NAV_HPPOSLLH_data_t`, etc.) is undesirable.
It would be much better to use a single common type for the callback parameter.

In v4, the callback parameter should be a new type: `ubxCallbackDataCommon_t`

Please see the new example `examples\CallbackExample1_NAVHPPOSLLH\CallbackExample1_NAVHPPOSLLH.ino`

The callback is to be defined as:

```void printPVTdata(ubxCallbackDataCommon_t *theData)```

Create `ubxCallbackDataCommon_t`. It will need to be a new `struct` which includes - for example - the message Class and ID, an emum representing the type of data structure (`UBX_NAV_PVT_data_t`, `UBX_NAV_HPPOSLLH_data_t`, etc.).

`ubxMessage` currently defines `_callbackPtr` as `void (*_callbackPtr)(uint8_t *)`. This will need to be changed to `void (*_callbackPtr)(ubxCallbackDataCommon_t *)`

### getCallbackDataStruct Factory design pattern

Add whatever code is necessary to make it possible to do the following in the callback:

```
void printPVTdata(ubxCallbackDataCommon_t *theData)
{
    auto theDataStruct = getCallbackDataStruct(theData);

    auto timeOfWeek = getFieldFromCallbackDataStruct(theDataStruct, "iTOW");
    Serial.print(F("TimeOfWeek: "));
    Serial.print(timeOfWeek); // Print the Time Of Week
    Serial.print(F(" (ms)"));
}
```

I envisage `getCallbackDataStruct` as being a Factory method / design pattern which returns enough information to make `auto timeOfWeek = getFieldFromCallbackDataStruct(theDataStruct, "iTOW");` possible. The return type of `getCallbackDataStruct` will need to contain enough information so that `getFieldFromCallbackDataStruct` can navigate to the `_storage` of the `ubxNAVHPPOSLLH` and extract the "iTOW" as `UBX_CFG_U4` (`uint32_t`).

`getFieldFromCallbackDataStruct` will need to:
- Step through each registered message type
- Compare the `_Class` and `_ID` of the message type to the class and ID stored in return type of `getCallbackDataStruct`
- If a match is found:
    - The code should step through the `const ubxField ubxFields[]` for that message
    - Use `extractValue()` to extract the value for the selected Class, ID and `fieldName`, returning it in a `ubxAnyType`
    - Copy the value from `ubxAnyType` into the return type

I envisage `getFieldFromCallbackDataStruct` will also need to use a Factory method / design pattern to handle the different return types

If this is not possible, identify the nearest alternative strategy which is possible.

### setCfgValset

src\u-blox_GNSS.h contains a template method: `template <typename T> bool addCfgValset(uint32_t key, T value)`

Add a new template method named `setCfgValset`:

```
  template <typename T>
  bool setCfgValset(uint32_t key, T value, uint8_t layer = VAL_LAYER_RAM_BBR) // Set the single key to the given value using CFG-VALSET
  {
    newCfgValset(layer);
    addCfgValset(key, value);
    return sendCfgValset();
  }
```

### class ubxMessage needs separate callback storage

`class ubxMessage` defines `_storage`: `uint8_t *_storage = nullptr;    // Raw payload storage - nullptr until initStorage() is called`

It needs separate storage for the callback copy / copies

Add the following to `ubxMessage`:
- `uint8_t *_callbackStorage = nullptr; // Storage for the callback copy / copies - nullptr until initCallbackStorage() is called`
- `initCallbackStorage()`

The code for `initCallbackStorage()` will be:

```
    // Lazily allocate _callbackStorage - only when the message is actually used. This is how "delete the
    // header, save the RAM" (AGENTS.md) is meant to work for a message that is never instantiated
    // at all - see ubxMessageVector.h.
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
```

`_numCallbackCopies` is usually 1, but RXM-SFRBX and ESF-MEAS will require multiple callback copies. We are not yet ready to add RXM-SFRBX and ESF-MEAS to the `ubxMessage` `class`. This will be added at a future date.

### setAutoCallbackPtr

The v3 library contains a setAuto-callbackPtr method for individual messages: `setAutoPVTcallbackPtr`, `setAutoNAVHPPOSECEFcallbackPtr`.
It would be much better to be able to use a single method named `setAutoCallbackPtr`.
Add the code for `void (*callbackPointerPtr)(ubxCallbackDataCommon_t *), const char *classStr, const char *idStr, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait)`.

The code will:
- Steps through each registered message type
- Look for a match for `classStr` and `idStr`
- If a match is found:
    - `callbackPointerPtr` is copied into the `ubxMessage` `_callbackPtr`
    - call `initStorage`
    - call `initCallbackStorage`

After `setAutoCallbackPtr` has been added, the individual setAuto-callbackPtr methods should be removed.

### Future work

If the message is periodic, and has a callback defined (`_callbackPtr` is not `nullptr`):
- `_storage` will be enlarged to hold both the current message and a copy of that message for the callback
- `processUBXpacket` should also copy the message contents into that callback copy

This is future work and will need to be added when callbacks are properly integrated into `ubxMessage`


## Test

Compile the example code in examples/Example1\_PositionVelocityTime using the batch file compile\_example.bat.
Parse the Docker (Dockerfile) output. Check that the example compiles successfully.
