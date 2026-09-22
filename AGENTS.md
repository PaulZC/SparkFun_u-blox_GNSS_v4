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

`getNAVPVT()`, `getLatitude()`, `getLongitude()` and `getAltitudeMSL()` are simple helper methods defined within the parent `class DevUBLOXGNSS`.
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

By default, `getNAVPVT()` will Poll the NAV\-PVT message. The code for `getNAVPVT()` could be:

```
bool DevUBLOXGNSS::getNAVPVT(uint16_t maxWait)
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
  - `getNAVPVT()` returns `true` if a NAV\-PVT message was received within `maxWait`, `false` otherwise
  - `getNAVPVT()` is blocking in this case. It will wait / stall for the full `maxWait` if needed
- If the NAV\-PVT message is periodic:
  - `getUBX()` will call the method `checkUbloxInternal()` to check for the availablelity of new I2C/UART/SPI data
  - If a new NAV\-PVT message has arrived, it is stored in the allocated storage
  - `getNAVPVT()` returns true if `checkUbloxInternal()` provided a new NAV\-PVT message, `false` otherwise.
  - In this mode, `getNAVPVT()` acts as a one-shot: `getNAVPVT()` will return `true` once on the arrival of new NAV-PVT data, clearing its internal `_moduleQueried` flag as it does so. The user should read whatever fields they need inside that same `if (getNAVPVT())` block, since the flag will not be `true` again until the next fresh message arrives - see `PeriodicExample1_NAVHPPOSLLH.ino`.
  - `getNAVPVT()` is non\-blocking in this case. After calling `checkUbloxInternal()` once, it returns `true` or `false` immediately. It does not wait for `maxWait` milliseconds.

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
- `src/ubxMessages/ubxNAVPVT.h` \- a concrete per\-message subclass (NAV\-PVT)
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
  longer clearing anything \- `getUBX()`/`getNAVPVT()` itself must clear the flag immediately after reporting
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

- If the message is not Periodic ("Automatic") and is being Polled each time, `getNAVPVT()` should Poll (request) the NAV\-PVT message and return `true` if it is received within the `maxWait` timeout. `getLatitude()` will return the most recent `data.lat`, even if it is "stale" (has been read before).
- If the message is Periodic, `getNAVPVT()` returns `true` if a new NAV\-PVT message has arrived since the last call. `getLatitude()` will return the most recent `data.lat`, even if it is "stale" (has been read before).

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

### getUbxMessagePtr Factory design pattern

Add whatever code is necessary to make it possible to do the following in the callback:

```
void printPVTdata(ubxCallbackDataCommon_t *theData)
{
    ubxMessage *msg = myGNSS.getUbxMessagePtr(theData);

    unsigned long timeOfWeek = (unsigned long)myGNSS.getUbxMessageFieldCallback(msg, "iTOW");
    Serial.print("TimeOfWeek: ");
    Serial.print(timeOfWeek); // Print the Time Of Week
    Serial.print(" (ms)");
}
```

(`getUbxMessagePtr`/`getUbxMessageFieldCallback` are `DevUBLOXGNSS` member methods, not free functions - call them as `myGNSS.getUbxMessagePtr(...)` etc. Inside a callback, use the `...Callback` variant - see "getUbxMessageField() has been split into two functions" below.)

I envisage `getUbxMessagePtr` as being a Factory method / design pattern which returns enough information to make `myGNSS.getUbxMessageFieldCallback(msg, "iTOW")` possible. The return type of `getUbxMessagePtr` will need to contain enough information so that `getUbxMessageField` can navigate to the `_storage` of the `ubxNAVHPPOSLLH` and extract the "iTOW" as `UBX_CFG_U4` (`uint32_t`).

`getUbxMessageField` will need to:
- Step through each registered message type
- Compare the `_Class` and `_ID` of the message type to the class and ID stored in return type of `getUbxMessagePtr`
- If a match is found:
    - The code should step through the `const ubxField ubxFields[]` for that message
    - Use `extractValue()` to extract the value for the selected Class, ID and `fieldName`, returning it in a `ubxAnyType`
    - Copy the value from `ubxAnyType` into the return type

I envisage `getUbxMessageField` will also need to use a Factory method / design pattern to handle the different return types

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

### Registry-first dispatch in `autoLookup` and `processUBXpacket`

The "Future work" above has been superseded by a broader change: `autoLookup()` and `processUBXpacket()` now both check the new registry *first*, before falling back to the old per-message code.

`autoLookup(Class, ID, maxSize)`:
- Calls `ubxMessages.find(Class, ID)`. If the message is registered, `maxSize` is set from `ubxMessagePtr->_messageLength` and the function returns `(ubxMessagePtr->_storage != nullptr)` immediately.
- Only messages *not yet* in the registry fall through to the old per-message `switch`/`if` chain below.
- **Now fully cleaned up**: the switch below only contains cases for messages that are genuinely still outside the registry (variable-length/repeated-block messages) - see "Messages still outside the registry" below. There is no remaining dead code here for any registered message.

`processUBXpacket(msg)`:
- Calls `ubxMessages.find(msg->cls, msg->id)`. If the message is registered:
  - Marks `_moduleQueried = true`
  - Copies the payload into `_storage` (if allocated)
  - Copies the payload into `_callbackStorage` and sets `_callbackDataValid` (if a callback is registered - see "class ubxMessage needs separate callback storage" above)
  - Copies into the file buffer if `_addToFileBuffer` is set
  - **This is exclusive** - the old per-message `switch (msg->cls) { ... }` chain below only runs in the `else` branch, i.e. only for messages *not* in the registry.
- **Now fully migrated**: `NAV_RELPOSNED`, `NAV_DAHEADING`, `RXM_COR`, and `MON_HW` - previously left alone because they didn't match the simple "standard pattern" (see below) - have since been migrated into the registry too, and their old special-case branches (M8/F9 length branching for RELPOSNED, version-byte branching for DAHEADING, old-style `callbackData` writes for RXM_COR) have been removed from the `switch`. See "Known issue: fixed-length `memcpy` ignores the actual received length" below for a correctness risk this specific migration introduced - filed as a recommendation in the `v4-migration-status` project doc rather than fixed here.

**Messages still outside the registry:** `NAV_SAT`, `NAV_SIG` (variable-length, repeated per-SV blocks), `RXM_SFRBX`, `RXM_RAWX`, `RXM_QZSSL6`, `RXM_MEASX`, `RXM_PMP`, `MON_COMMS`, `ESF_MEAS`, `ESF_RAW`, `ESF_STATUS`, `MGA_ACK_DATA0`, `MGA_DBD`, `SEC_SIG` - these have variable-length or repeated-block payloads that the fixed-length, fixed-field-table registry (`ubxField[]` + a single `_messageLength`) doesn't yet model, so they remain on the old per-message `packetUBXxxx` pointers and dedicated parsing code in both `autoLookup()` and `processUBXpacket()`. Extending the registry to support these is future work.

**Legacy per-field getters have been rewritten to use the registry.** Every individual per-field getter for a registered message (`getYear()`, `getMonth()`, `getLatitude()`, `getLongitude()`, `getAltitudeMSL()`, `getHorizontalAccEst()`, `getSIV()`, `getFixType()`, `getRoll()`, `getGeometricDOP()`, etc.) now calls `getUBXfield(Class, ID, "fieldName", &value, maxWait)` and returns the appropriate `ubxAnyType` union member, instead of reading the old `packetUBXxxx->data.xxx` struct field directly. This resolves the "accepted, deliberate migration debt" noted in an earlier revision of this document (the legacy getters returning stale/zero values once `processUBXpacket()` stopped populating the old structs) - the getters now read live registry data like everything else. `getUBXfield()` itself does not poll; the caller is expected to have called `getUBX()` (or a wrapper such as `getNAVPVT()`) first so fresh data is actually in storage.

**`getUBX()` and `setAutoUBX()`/`setAutoUBXrate()` are now fully generic**, with no per-struct bookkeeping left:
- `bool getUBX(uint8_t Class, uint8_t ID, uint16_t maxWait)` drives the poll-vs-automatic decision entirely through `ubxMessages.initStorage()`, `ubxMessages.isAutomatic()`, `ubxMessages.implicitUpdate()`, and `ubxMessages.moduleQueried()`/`setModuleQueried()` - see the numbered steps earlier in this section, which this now matches exactly.
- A name-based overload, `bool getUBX(const char *Class, const char *ID, uint16_t maxWait)`, looks the message up with `ubxMessages.findByName()` and forwards to the numeric overload. `Example1_PositionVelocityTime.ino` demonstrates this: `myGNSS.getUBX("NAV","PVT")` followed by `ubxMessages.findByName("NAV","PVT")` and `getUbxMessageField(msg, "lat")` (see below).
- `setAutoUBX(Class, ID, enabled, [implicitUpdate,] layer, maxWait)` (numeric and name-based overloads) and `setAutoUBXrate(Class, ID, rate, implicitUpdate, layer, maxWait)` (numeric and name-based) replace the old per-message `setAutoPVTrate()`-style functions. `setAutoUBXrate()` uses `ubxMessages.getMsgOutKey()` + `setVal8()` to set the message rate, with a three-tier strategy for keeping the registry's `_automatic` flag accurate: (1) if `setVal8()` succeeds, the flag is set from the requested rate; (2) if it fails, the actual rate is read back with `getVal8()` and the flag is set from that instead; (3) if both fail, the flag is set from the requested rate anyway, so a transient write/read failure (e.g. I2C congestion) can't silently strand the flag in a state where `getNAVPVT()`-style wrappers return `false` forever.

**`getUbxMessageField()` has been split into two functions** in `ubxMessage.h`, both reading through the same shared `extractFieldFrom()` core:
- `getUbxMessageField(ubxMessage *theMessage, const char *fieldName)` reads from the message's live `_storage` - the counterpart to the new `getUBX()` + `ubxMessages.findByName()` polling pattern shown in `Example1_PositionVelocityTime.ino`.
- `getUbxMessageFieldCallback(ubxMessage *theMessage, const char *fieldName)` reads from the message's frozen `_callbackStorage` snapshot - for use *inside* a registered callback, where `_storage` may already have been overwritten by the next incoming message before the callback runs. `CallbackExample1_NAVHPPOSLLH.ino` uses this one.
- Both use the same `0xFF` "field not found" sentinel `ubxDataType` convention, and both are read through `getUbxMessagePtr(theData)` (the renamed factory function - see "getUbxMessagePtr Factory design pattern" above) to get from a callback's `ubxCallbackDataCommon_t *` to the `ubxMessage *` in the first place.
- Callers should use the `_storage` variant when polling (`getUBX()`) and the `_callbackStorage` variant when inside a callback fired by `checkCallbacks()`; using the wrong one compiles fine but reads the wrong (or stale) copy.

### Known issue: fixed-length `memcpy` ignores the actual received length

`processUBXpacket()`'s registry branch copies a registered message's payload with `memcpy(ubxMessagePtr->_storage, msg->payload, ubxMessagePtr->_messageLength)` (and the same for `_callbackStorage`) - using the message's registered, fixed `_messageLength`, not the incoming packet's actual `msg->len`. This is safe for a message whose wire length never varies, but not for one where it can (e.g. `NAV_RELPOSNED`, registered at the F9 length but sent shorter by M8-generation receivers). A correctly-length-gated version of this copy already exists and is unused: `ubxMessageVector::storePayload()` in `ubxMessageVector.h` takes the real `len`, clamps it to `_messageLength`, and `memcpy`s only that many bytes - but `processUBXpacket()` does its own inline `memcpy` instead of calling it. See the `v4-migration-status` project doc for the full writeup and severity assessment; this has not been changed here per instruction.

## Message Class self-registration

The code in `C:\Users\pc235\Documents\GitHub\flux-sdk` contains a very useful self-registration feature.
Each device in the sub folder `src\device` is able to self-register.

`flxDeviceFactory` is a singleton that holds a multimap of device "builders," keyed by I2C address + confidence.
`DeviceBuilder<DeviceType>` is a template whose constructor calls `flxDeviceFactory::get().registerDevice(this)`.
The macro `#define flxRegisterDevice(kDevice) static DeviceBuilder<kDevice> global_##kDevice##Builder;` declares a file-scope static object of that builder template.

At the bottom of (e.g.) `flxDevACS37800.cpp`:

```
flxRegisterDevice(flxDevACS37800);
```

expands to `static DeviceBuilder<flxDevACS37800> global_flxDevACS37800Builder;` — a global object. In C++, static/global objects are constructed before main() runs, as part of static initialization. So merely linking that translation unit into the binary is enough to make its constructor run and call registerDevice() — no explicit call site, no central switch statement, no list of "known devices" to edit.

At runtime, `flxDeviceFactory::buildDevices(i2cDriver)` scans the I2C bus, and for each registered builder calls its `isConnected()/create()` to instantiate only the devices that are actually plugged in.

I want to use the same or a similar technique in the v4 of this library.

Can we remove the code from the `ubxMessageVector` constructor that start with:

```
        ubxMessageVectors.push_back(new ubxNAVPVT());
        ubxMessageVectors.push_back(new ubxNAVPOSECEF());
        ubxMessageVectors.push_back(new ubxNAVPOSLLH());
        ubxMessageVectors.push_back(new ubxNAVSTATUS());
```

and replace it with the self-registration technique?

For `ubxMessageVectors.push_back(new ubxNAVPVT());` to be able to compile, the `ubxNAVPVT.h` file must be present and `class ubxNAVPVT` must be defined.

Can we change it so that: if `ubxNAVPVT.h` is included (`#include "ubxNAVPVT.h"`), `class ubxNAVPVT` self-registers?

Such that: to remove support for (e.g.) NAV-PVT, all that would be required would be to comment the one line `#include "ubxNAVPVT.h"`. If `#include "ubxNAVPVT.h"` is commented, the code compiles successfully and runs normally but without NAV-PVT suppport.

## nmeaMessage

The `nmeaMessage` `class` follows the structure of `ubxMessage` as closely as possible.

NMEA Messages are all ASCII text. To avoid difficulties with `anyType` code, data extracted by `getNmeaMessageField` / `getNmeaMessageFieldCallback` is returned as `String`. The standard C functions `atoi`, `atof` and `strtod` cover most use cases for converting it to other types - see `PeriodicExample1_GPGGA.ino` and `PollingExample1_GPZDA.ino`.

All 8 standard-length NMEA messages now have their own `nmea*.h` file in `src/nmeaMessages/`: `nmeaGGA.h`, `nmeaDTM.h`, `nmeaGLL.h`, `nmeaGNS.h`, `nmeaGST.h`, `nmeaRMC.h`, `nmeaVTG.h`, `nmeaZDA.h`. Each self-registers with `nmeaRegisterMessage(...)`, exactly like the `ubxRegisterMessage(...)` messages - see "Message Class self-registration" above.

The `nmeaMessage` class has replaced the existing NMEA support from v3 of the library. `DevUBLOXGNSS` now holds a public `nmeaMessageVector nmeaMessages;` registry, and a generic (msgId)-keyed API that mirrors the (Class, ID)-keyed UBX API function-for-function: `getNMEA`, `getNMEAfield`, `setAutoNMEA`/`setAutoNMEArate`, `assumeAutoNMEA`, `flushNMEA`, `logNMEA`, `setNmeaCallbackPtr`, and a new `pollNMEA(msgId, maxWait)` (the NMEA equivalent of `sendCommand` - it sends `$EIGNQ,<msgId>*<checksum>` to poll a single message using the GN Talker ID). The old v3 per-message NMEA API (`getLatestNMEAGPGGA()`-style getters, `storageNMEAGPGGA`-style pointers, per-message `setNMEA<MSG>callbackPtr()` setters) has been removed entirely.

See `PeriodicExample1_GPGGA.ino` (a message that is periodic/automatic by default), `PollingExample1_GPZDA.ino` (a message that is not, and so must be polled), and `CallbackExample2_GPRMC.ino` (a message read via a registered callback instead of polling in `loop()`) for the three usage patterns.

`extractFieldFrom()`'s `DDMM`/`DDDMM` cases (used for `lat`/`lon`) always compute at least 3 decimal places of degrees, even if the NMEA sentence's minutes field has no fractional digits at all - converting minutes to degrees divides by 60, so truncating to the number of fractional digits actually present in the sentence would silently throw away precision in the whole-minutes part of the conversion, not just the fraction.

Future work will be to include the variable length GSA and GSV messages.

## Adding the variable-length UBX messages

We need to add the variable length UBX messages to the `ubxMessages` `class`. E.g. UBX-NAV-SAT currently uses the v3 legacy code and needs both `UBX_NAV_SAT_t` and `packetUBXNAVSAT`. The critical issue for (e.g.) NAV-SAT is that: `UBX_NAV_SAT_t` includes a single `UBX_NAV_SAT_data_t`; `UBX_NAV_SAT_data_t` includes a single `UBX_NAV_SAT_header_t` and up to `UBX_NAV_SAT_MAX_BLOCKS` `UBX_NAV_SAT_block_t` stored in an array. How do we include NAV-SAT in `ubxMessages` and make the following callback possible:

```
// Callback: newNAVSAT will be called when new NAV SAT data arrives
void newNAVSAT(ubxCallbackDataCommon_t *theData)
{
  Serial.println();

  ubxMessage *msg = myGNSS.getUbxMessagePtr(theData);

  Serial.print(F("New NAV SAT data received. It contains data for "));
  uint8_t numSvs = myGNSS.getUbxMessageFieldCallback(msg, "numSvs");
  Serial.print(numSvs);
  if (numSvs == 1)
    Serial.println(F(" SV."));
  else
    Serial.println(F(" SVs."));

  // Print the signal strength for each SV as a barchart
  for (uint8_t block = 0; block < numSvs; block++) // For each SV
  {
    auto blockPointer = getNAVSATSVblock(msg, block);
    switch (myGNSS.getUbxNavSatBlockData(blockPointer, "gnssId")) // Print the GNSS ID
    {
      case 0:
        Serial.print(F("GPS     "));
      break;
      case 1:
        Serial.print(F("SBAS    "));
      break;
      case 2:
        Serial.print(F("Galileo "));
      break;
      case 3:
        Serial.print(F("BeiDou  "));
      break;
      case 4:
        Serial.print(F("IMES    "));
      break;
      case 5:
        Serial.print(F("QZSS    "));
      break;
      case 6:
        Serial.print(F("GLONASS "));
      break;
      default:
        Serial.print(F("UNKNOWN "));
      break;      
    }
    
    uint8_t svId = myGNSS.getUbxNavSatBlockData(blockPointer, "svId"); // Extract the svId from the block
    Serial.print(); // Print the SV ID
    
    if (svId < 10) Serial.print(F("   "));
    else if (svId < 100) Serial.print(F("  "));
    else Serial.print(F(" "));

    // Print the signal strength as a bar chart
    for (uint8_t cno = 0; cno < myGNSS.getUbxNavSatBlockData(blockPointer, "cno"); cno++)
      Serial.print(F("="));

    Serial.println();
  }
}
```

Refer to `u-blox-X20-HPG-2.10_InterfaceDescription_UBXDOC-304424225-21263.pdf` for the UBX-NAV-SAT field names and definitions.

Please write a proposal on how we should do that. Do not make any code changes yet. Let me review your proposal first.

## Adding support for RXM-SFRBX

We need to add the variable-length RXM-SFRBX message to the `ubxMessages` `class`. This will need care.

The u-blox GNSS modules output several RXM-SFRBX messages as a 'group', with no bus delay or separation between them.
Those multiple messages may all be processed by a single call of `checkUblox()`.
The `ubxMessage` `_callbackStorage` needs to be able to hold those multiple messages.

In the v3 library, the "automatic" storage for RXM-RAWX included `UBX_RXM_SFRBX_CALLBACK_BUFFERS`. 14 buffers was found to work well.

We need to include the same feature in v4.

In `class ubxRXMSFRBX`, use `const uint8_t numCallbackCopies = 14;`.

I suggest doing the following:
In `class` `ubxMessage`, change `_storageCallback` so that it becomes a ring buffer of message buffers: `_numCallbackCopies`, each `_messageLength` in size.
Include a `_head`, and a `_tail` to make it easy to determine how many buffers contain fresh not-read-before data.
The `class ubxMessageVector` `storePayload` method will need to be modified so that it writes the incoming message to the `_head` (if space is available).
It may be necessary to add a `extractPayload` method to `ubxMessageVector` to extract the message pointed to by the `_tail`.

Please write a proposal for how you will support for RXM-SFRBX.
Do not make any code changes yet. Write the proposal first. I will need to approve it before you change the code.

Sidenote: ESF-MEAS will need the same multiple-buffer approach. It requires (at least) 6 callback buffers.

### Validated on hardware (ZED-X20P) - numbers above were optimistic

`DataloggingExample1_RAWX_and_SFRBX` on a ZED-X20P showed real traffic exceeding both of this
section's original estimates, taken from the v3 library / interface description:

- **`UBX_RXM_SFRBX_CALLBACK_BUFFERS`** (the "14 buffers was found to work well" figure above) had
  to be raised to **50** to avoid data loss - the X20P outputs SFRBX in back-to-back groups of
  more than 40 messages, not the handful the v3 figure assumed.
- **`UBX_RXM_SFRBX_MAX_WORDS`** had to be raised from 16 (the interface description's stated
  maximum for protocol version 17) to **20** - the X20P has been observed sending SFRBX messages
  with 19 data words, above the documented maximum.

Both constants now live in `u-blox_structs.h` as named constants (`UBX_RXM_SFRBX_CALLBACK_BUFFERS`,
`UBX_RXM_SFRBX_MAX_WORDS`), not literals inside `ubxRXMSFRBX.h`, specifically so they can be
tuned like this without touching the message class itself. Validated by the user: the number of
SFRBX messages seen by the registered callback matched the number of SFRBX messages actually
logged to the `RXM_RAWX.ubx` file on the SD card, over a real logging run.

**Bearing on ESF-MEAS (tomorrow's planned work, as of this note):** the "(at least) 6 callback
buffers" estimate above should be treated the same way the SFRBX "14 buffers" estimate was -
a starting point, not a hard target. Worth using a named, easily-tunable constant for ESF-MEAS's
buffer count too, and checking real hardware behavior before assuming the documented/estimated
figure holds.

## Adding support for NMEA GSV messages

Please add support for NMEA GSV.

Refer to `u-blox-X20-HPG-2.10_InterfaceDescription_UBXDOC-304424225-21263.pdf` for the NMEA GSV field names and definitions.

NMEA GSV messages are a special case. To support them, `nmeaMessages` will need to be restructured so that it can support: variable length messages; and multiple `_storageCallback`.

The GNSS module will output multiple xxGSV messages in each navigation cycle. The library needs to be able to process and store all those messages from a single call of `checkUblox()`.

Each individual GSV message is variable length:
* The 'header' comprises: `xxGSV`, `numMsg`, `msgNum`, `numSV`
* Followed by 1 to 4 blocks of: `svid`, `elv`, `az`, `cno`
* Followed by a 'footer' of: `signalId`

For each constellation, up to 9 messages can be generated: `numMsg` is the total number of messages in this group for the given constellation; `msgNum` is number of this message within the group.

The Talker ID ("GP", "GL", "GA", "GB", "GI", "GQ") is the first two letters of the `xxGSV`. E.g. "GPGSV" messages contain the satellite information for the GPS constellation.

Please modify `nmeaMessages` so it supports variable length messages.

In `class nmeaGSV`, I suggest you add:
* A new `const` member named `numBlockFields`. Set it to 4.
* A new array of `struct` `const nmeaField nmeaBlockFields[numBlockFields]`
* A new `const` member named `numHeaderFields`. Set it to 4. This also defines the position of the first field of the first block.
* A new `const` member named `maxNumBlocks`. Set it to 4. (The minimum number of blocks (= 1) is implied.)

Interpret `numFields` as excluding the `numBlockFields`

Reinterpret the `nmeaField` `fieldNumber` so that it represents the position in the header or footer, skipping the variable block. E.g.:
```
    const nmeaField nmeaFields[numFields] = {
        {"xxGSV", nmeaDataTypeString, 0}, // Header (implied by fieldNumber < numHeaderFields)
        {"numMsg", nmeaDataTypeDigit, 1}, // Header (implied by fieldNumber < numHeaderFields)
        {"msgNum", nmeaDataTypeDigit, 2}, // Header (implied by fieldNumber < numHeaderFields)
        {"numSV", nmeaDataTypeNumeric, 3}, // Header (implied by fieldNumber < numHeaderFields)
        {"signalId", nmeaDataTypeString, 4}, // Footer (implied by fieldNumber >= numHeaderFields)
    };
```

Add:
```
    const nmeaField nmeaBlockFields[numBlockFields] = {
        {"svid", nmeaDataTypeNumeric, 0},
        {"elv", nmeaDataTypeNumeric, 1},
        {"az", nmeaDataTypeNumeric, 2},
        {"cno", nmeaDataTypeNumeric, 3},
    };
```

`signalId` could be Hexadecimal. It needs `nmeaDataTypeString`.

Set `numCallbackCopies` to 54 (six constellations * up to 9 messages per constellation)

You will need to adjust `nmeaMessage` `extractFieldFrom` so it supports the same or similar parameters `fieldsOverride` and `numFieldsOverride` as for `ubxMessage`.

For the callbacks, use the same transient `_callbackReadIndex` technique as `ubxMessage`.

In the callback for GSV, it needs to be possible to use:
```
int svid = atoi(myGNSS.getNmeaMessageBlockFieldCallback(msg, block, "svid").c_str());
```
If `block` is illegal ( >= `maxNumBlocks`), `getNmeaMessageBlockFieldCallback` should return an empty `String`.

Add `getNmeaMessageBlockField()` to support field extraction from `_storage`.

## Debug printing (SfeDebugPrint)

`DevUBLOXGNSS::debugPrint()`/`debugPrintln()` used to be plain member functions of `DevUBLOXGNSS`,
so nothing else in the library could call them - including `ubxMessageVector`/`nmeaMessageVector`,
whose own methods (e.g. `ubxMessageVector::storePayload()`'s ring-buffer-full branch) sometimes
have something worth reporting but no way to say it.

They now live on a small shared base class, `SparkFun_UBLOX_GNSS::SfeDebugPrint` (`src/sfe_debug.h`/
`.cpp`), which `DevUBLOXGNSS`, `ubxMessageVector` and `nmeaMessageVector` each inherit independently.
`sfe_debug.h` has to be its own leaf header (depending only on `sfe_bus.h`, for the `SfePrint` type)
because `u-blox_GNSS.h` includes `ubxMessageVector.h`/`nmeaMessageVector.h`, not the other way
around - those two headers cannot include `u-blox_GNSS.h` back, so they cannot inherit from
`DevUBLOXGNSS` directly.

This is real (non-static) inheritance, so each of the three objects gets its own separate copy of
`_debugSerial`/`_printDebug`/`_printLimitedDebug`. That matters because `ubxMessages`/`nmeaMessages`
are owned BY VALUE as member objects of `DevUBLOXGNSS` (the opposite containment direction from
inheritance) - so on its own, a `ubxMessages.debugPrint(...)` call would compile but never print
anything, since `myGNSS.enableDebugging()` only ever touches `DevUBLOXGNSS`'s own inherited copy of
that state. `static` shared state would "fix" that, but would incorrectly couple debug on/off
across every `SFE_UBLOX_GNSS` instance in a sketch that uses more than one.

Instead, `SfeDebugPrint::copyDebugStateFrom(const SfeDebugPrint &other)` copies just the debug
state (port + both enable flags) from one `SfeDebugPrint` to another. `DevUBLOXGNSS::enableDebugging()`
and `disableDebugging()` call it on `ubxMessages`/`nmeaMessages` right after updating their own
(inherited) state, so all three stay in sync per-`DevUBLOXGNSS`-instance without any shared/static
state. If you add a fourth class that needs `debugPrint()`/`debugPrintln()`, inherit
`SfeDebugPrint` and remember to wire it into `enableDebugging()`/`disableDebugging()` the same way -
inheriting the class alone is not enough to make it ever actually print.

## Adding support for ESF-MEAS

It is time to migrate ESF-MEAS into `ubxMessages`. It needs a revised strategy since ESF-MEAS:
* Is variable-length, containing multiple `data` groups / blocks (`numMeas` in total)
* Can include an optional `calibTtag` time tage group (similar to the NMEA "footer")
* Requires multiple `_callbackStorage` (`UBX_ESF_MEAS_CALLBACK_BUFFERS`) (found during earlier hardware tests)

It is not clear if the GNSS module outputs `calibTtag`, or whether that is only added on messages sent **to** the GNSS.
For safety, we should assume that ESF-MEAS output by the GNSS can include `calibTtag`.

Please write a proposal on how ESF-MEAS will be added and how `class` `ubxMessage` will need to be adapted to include it.
Please do not make any code changes yet. Let me review your proposal first.

### Implemented

The proposal was approved and implemented in the same session - see `esf-meas-proposal.md`/`.pdf`
in the repo root for the full design, and `claude/v4-migration-status.md` (Phase 30) for the
as-built record. Summary of what shipped:

- `class ubxESFMEAS` (`src/ubxMessages/ubxESFMEAS.h`) - self-registered like every other
  variable-length message, following the NAV-SAT/RXM-SFRBX/SEC-SIG pattern.
- `class ubxMessage` gained four GENERAL, additive capabilities (nullptr/0 for every message
  registered before ESF-MEAS, so no other message is affected):
  - **Actual-length tracking**: `_actualLength`/`_callbackActualLength[]`, set by
    `ubxMessageVector::storePayload()` from the real received byte count. Needed because
    `_storage`/`_callbackStorage` are never zeroed between messages, so bytes beyond a shorter
    message's length can hold stale data from a previous, longer one - a message whose own header
    count field cannot be trusted (like ESF-MEAS's `numMeas`) needs this to know where real data
    actually ends.
  - **Defensive block count**: `_blockCountField` (set via `addClassID()`) + `getBlockCount()` -
    the minimum of the header field's own value, how many whole blocks fit in the actual received
    length, and `_maxBlocks`. New accessors `getUbxMessageBlockCount()`/`...Callback()`.
  - **Optional footer group**: `_footerFields`/`_numFooterFields`/`_footerLength` (set via
    `addClassID()`) + `extractFooterFieldFrom()`, whose offset is computed from the defensive block
    count above, not `_maxBlocks`. Returns "field not found" for a message that did not actually
    include the footer, distinct from a footer value that happens to be zero. New accessors
    `getUbxMessageFooterField()`/`...Callback()`.
  - **Raw-frame relay** (added at the user's request during proposal review, beyond ESF-MEAS
    itself): `_callbackRawFrame` - a per-ring-slot buffer holding the COMPLETE raw UBX frame (sync
    bytes + Class/ID/length + payload + checksum), synthesized by `writeCallbackRawFrame()`. Lets a
    sketch relay a message verbatim from inside a callback (e.g. `Serial2.write(ptr, len)`) without
    reconstructing it by hand. New accessors `getUbxMessageRawLengthCallback()`/
    `getUbxMessageRawPtrCallback()`. This is automatic for ANY message with a callback registered,
    not opt-in - roughly doubles that message's `_callbackStorage` RAM cost, which is worth
    remembering when choosing `numCallbackCopies` for a future high-volume message.
- `storePayload()` (`ubxMessageVector.h`) gained two required parameters, `checksumA`/`checksumB`,
  needed to synthesize the raw frame - its one call site (`processUBXpacket()`) already has them
  from the incoming `ubxPacket`.
- `getSensorFusionMeasurement()` is redacted entirely, per explicit instruction - it took the old
  v3-style `UBX_ESF_MEAS_data_t` by value, and nothing constructs one any more.
- `UBX_ESF_MEAS_t`/`ubxESFMEASAutomaticFlags` (the old v3 RAM-management wrapper) are removed from
  `u-blox_structs.h`; `UBX_ESF_MEAS_data_t`/`UBX_ESF_MEAS_sensorData_t` (the wire-format structs)
  are kept as documented reference, matching every prior migration. `UBX_ESF_MEAS_CALLBACK_BUFFERS`/
  `UBX_ESF_MEAS_MAX_LEN` are unchanged/still used, as `numCallbackCopies`/`messageLength`.
- `case UBX_CLASS_ESF:` stays alive in `autoLookup()`/`processUBXpacket()`/`checkCallbacks()` -
  unlike MON-COMMS/SEC-SIG, only the ESF-MEAS branch was removed from each, since ESF-RAW/
  ESF-STATUS still have live v3 code there.
- Found and fixed while touching this code again: `setAutoESFMEAS`/`setAutoESFMEASrate`/
  `setAutoESFMEAScallbackPtr`/`assumeAutoESFMEAS`/`logESFMEAS`/`initPacketUBXESFMEAS` were declared
  in `u-blox_GNSS.h` but had NO definitions anywhere in `u-blox_GNSS.cpp` - dead declarations that
  were never callable, predating this migration. Also fixed four stale "SEC-SIG (Version 2)"
  comments left over from the Version 3 discovery (see the "SEC-SIG" section above) that the
  original comment-only fix pass missed, in `u-blox_GNSS.h`/`u-blox_GNSS.cpp`, plus a stray "ESF RAW
  data cannot be polled" comment that had been sitting above the ESF-MEAS section in
  `u-blox_structs.h` (the correct copy of that comment is above the real ESF-RAW section).
- **Hardware-validated by the user:** added `CallbackExample10_ESFMEAS` (their own sketch, not
  written by Claude) and ran it on a real ZED-F9R over I2C. `timeTag`, `numMeas`-bounded block
  iteration, `dataType`-driven decoding, the 24-bit signed `dataField` (gyro rate, accelerometer
  force, gyro temperature, wheel/speed ticks all observed and printed with plausible values), and
  the `calibTtag` footer field (printed as "Rx Time", present and non-zero on every message
  observed) all decoded correctly across a continuous run - confirming the field tables, the
  X4/U4-tagged 24-bit `dataField` sub-field, and `extractFooterFieldFrom()`'s footer-presence
  detection are all correct against real hardware, not just static reasoning.
- **Follow-up: the defensive block count is now hardware-validated across multiple block counts,
  including one real bug found and fixed along the way (a sketch mistake, not a library bug).**
  The user added a self-check comparing `numMeas` (the header field) against
  `getUbxMessageBlockCount()`, and initially saw spurious mismatches (always reporting 4,
  regardless of the real count). **Root cause: the sketch called the wrong accessor** -
  `getUbxMessageBlockCount(msg)` reads the message's LIVE `_storage`/`_actualLength` (for the
  separate `getUBX()`-polling pattern used outside a callback), not the frozen per-slot
  `_callbackStorage`/`_callbackActualLength` a queued callback is actually reporting on. Since
  ESF-MEAS is ring-buffered (`numCallbackCopies = UBX_ESF_MEAS_CALLBACK_BUFFERS = 6`) and arrives
  in a rapid, mixed-rate burst, `_storage` had almost always already been overwritten by a newer
  message (usually the high-rate 4-measurement gyro+temp one) by the time any queued callback got
  around to reading it - explaining both the mismatches and why they were so often exactly "4".
  **Fix (the sketch's, confirmed correct):** use `getUbxMessageBlockCountCallback(msg)` instead -
  the same live-vs-callback-storage split that already exists for
  `getUbxMessageField()`/`getUbxMessageFieldCallback()`. After the fix, `numMeas` and
  `getUbxMessageBlockCountCallback()` agreed on every message across a continuous run, correctly
  showing 4 (gyro X/Y/Z + temp), 3 (accel X/Y/Z), and 1 (speed ticks) - the defensive block-count
  formula is now confirmed correct for more than just the numMeas=1 case. **This is a genuinely
  easy mistake to repeat** (the two accessors differ only by "Callback" in the name) - worth
  remembering for any future message: inside a registered callback, always use the
  `...Callback()`-suffixed accessor, never the plain one, for the same reason this applies to
  every other field/block/footer getter.
- **`UBX_ESF_MEAS_CALLBACK_BUFFERS` is now hardware-validated too - and needed raising, same
  pattern as RXM-SFRBX's pre-Phase-26 buffer count.** The user tried 6 (the original estimate) and
  12, and saw ring-full `debugPrint` warnings (messages being dropped) at both; **18 produced no
  buffer errors** and is now the shipped value in `u-blox_structs.h`. The comment above the
  constant was updated to record this (previously said "has NOT yet been validated"). **The user's
  own words: "I am happy. ESF-MEAS is validated."**
- **Still open:** the raw-frame relay accessors (`getUbxMessageRawLengthCallback()`/
  `getUbxMessageRawPtrCallback()`) still haven't been exercised by any sketch. This implementation
  has also still NOT been compiled with `compile_example.bat` - Docker was unavailable in every
  sandbox tried so far, so the `compile_example.bat`/Dockerfile check described in "Test" below has
  not been run against it. Neither of these blocks calling ESF-MEAS itself validated - both are
  narrower loose ends (an unused accessor, and a build-tooling gap that has applied to every phase
  of this engagement, not something specific to ESF-MEAS).

## Adding support for ESF-RAW and ESF-STATUS

Please add UBX-ESF-RAW and UBX-ESF-STATUS to `class` `ubxMessages`. `getRawSensorMeasurement()`
and both overloads of `getSensorFusionStatus()` can be redacted. Both only need one
`_callbackStorage` buffer - ignore the comment about "the NEO-M8U sends them in sets of ten (i.e.
seventy readings per message)".

### Implemented

Implemented in the same session as a direct instruction (no proposal needed - neither message is
architecturally novel, same as MON-COMMS/SEC-SIG: both follow the ordinary header-plus-repeated-
blocks pattern already established by NAV-SAT/RXM-SFRBX/MON-COMMS/SEC-SIG/ESF-MEAS). See
`claude/v4-migration-status.md` (Phase 31) for the as-built record. Summary of what shipped:

- `class ubxESFRAW` (`src/ubxMessages/ubxESFRAW.h`) and `class ubxESFSTATUS`
  (`src/ubxMessages/ubxESFSTATUS.h`) - both self-registered, following the same variable-length
  pattern as every message above. Both set `numCallbackCopies = 1`, per explicit instruction - the
  "NEO-M8U sends sets of ten" note is about how many BLOCKS one single ESF-RAW message can
  contain (unchanged - `maxBlocks` is still sized for `DEF_NUM_SENS * DEF_MAX_NUM_ESF_RAW_REPEATS`
  = 70), not about needing multiple ring-buffer copies for a burst of separate messages, unlike
  RXM-SFRBX/ESF-MEAS.
- **ESF-RAW is a genuinely new case: it has NO block-count field anywhere in its wire format at
  all** - its 4-byte header is entirely reserved (the old v3 struct's own comment on
  `numEsfRawBlocks`: "this is not contained in the ESF RAW message. It is calculated from the
  message length."). Every previous variable-length message either has a trustworthy header count
  field the caller reads directly (MON-COMMS's `nPorts`, SEC-SIG's `jamNumCentFreqs`), or an
  unreliable one that still exists and gets cross-checked (ESF-MEAS's `numMeas`, via
  `_blockCountField`). ESF-RAW has neither - so `ubxMessage::getBlockCount()` (`ubxMessage.h`) was
  extended: when a message has NOT set `_blockCountField`, it now falls back to computing the
  block count purely from the actual received length (bounded by `_maxBlocks`), instead of
  unconditionally returning 0 as it did before. This is additive and backward compatible - every
  message that DOES set `_blockCountField` (only ESF-MEAS) behaves identically to before; every
  ordinary message that sets neither (MON-COMMS, SEC-SIG, ESF-STATUS) now also gets a working,
  purely-defensive `getUbxMessageBlockCount()`/`...Callback()` as a bonus, agreeing with its own
  header field under normal circumstances - though the intended way to bound their block loop
  remains reading the header field directly (`nPorts`/`jamNumCentFreqs`/`numSens`), per each
  message's own file-header comment. For ESF-RAW specifically, this fallback is not a bonus but
  the ONLY way to know how many blocks a particular message actually contains -
  `getUbxMessageBlockCount()`/`...Callback()` is the intended, and only, way to bound its block
  loop. `numFields` is 0 and `ubxFields` is passed as `nullptr` for ESF-RAW, since there is
  nothing in its 4-byte header worth exposing as a named field.
- ESF-STATUS's per-sensor block exposes both the raw status byte (`sensStatus1`/`sensStatus2`/
  `faults`) and its individual bit-packed sub-fields (`type`/`used`/`ready`/`calibStatus`/
  `timeStatus`/`badMeas`/`badTTag`/`missingMeas`/`noisyMeas`) - the same convention MON-COMMS uses
  for `txErrors` and SEC-SIG uses for `sigSecFlags`.
- `getRawSensorMeasurement()` and both overloads of `getSensorFusionStatus()` are redacted
  entirely, per explicit instruction - they took their data by value/pointer from the caller (the
  old v3-style `UBX_ESF_RAW_data_t`/`UBX_ESF_STATUS_data_t`), and nothing constructs one any more
  now that both messages' storage paths are the generic registry. No replacement helper was added -
  the generic `getUbxMessageBlockField()`/`getUbxMessageBlockFieldCallback()` API (with
  `getUbxMessageBlockCount()`/`...Callback()` to bound the loop) covers the same ground.
- `getESFSTATUS()` is kept as a thin wrapper (`return getUBX(UBX_CLASS_ESF, UBX_ESF_STATUS,
  maxWait);`), exactly matching `getMONCOMMS()`'s/`getSECSIG()`'s bodies - it is called directly by
  `getEsfInfo()` and possibly by sketches, unchanged. **There is no `getESFRAW()`** - ESF RAW data
  cannot be polled, it is "Output" only (a comment already correctly sitting above the real
  ESF-RAW section in `u-blox_structs.h`, fixed in place during the ESF-MEAS phase), and no such
  poll wrapper existed in the old v3 API either - this is a deliberate absence, not a gap.
- **Full migration of the old v3 scaffolding, same depth as MON-COMMS/SEC-SIG/ESF-MEAS:** removed
  `packetUBXESFRAW`/`packetUBXESFSTATUS` (the `UBX_ESF_RAW_t *`/`UBX_ESF_STATUS_t *` members) from
  `u-blox_GNSS.h`; removed the destructor's two cleanup blocks, the
  `autoLookup()`/`processUBXpacket()` branches for both messages under `case UBX_CLASS_ESF:` (the
  case label itself is now fully empty, matching `case UBX_CLASS_MON:`'s precedent - every message
  that was ever parsed there is now self-registered), and the `checkCallbacks()` manual
  callback-firing blocks for both - each replaced with a short retiring comment. Removed
  `setAutoESFSTATUS` (both overloads), `setAutoESFSTATUSrate`, `setAutoESFSTATUScallbackPtr`,
  `assumeAutoESFSTATUS`, `flushESFSTATUS`, `logESFSTATUS`, and `initPacketUBXESFSTATUS` entirely
  (declarations and definitions) - none survive even as thin wrappers, since the generic
  `setAutoUBX`/`setAutoUBXrate`/`setAutoCallbackPtr`/`assumeAutoUBX`/`flushUBX`/`logUBX` (by
  Class/ID or by name "ESF"/"STATUS") now do the same job.
- **Found while touching this code again: ESF-RAW's ENTIRE old "automatic" control-plane API was
  already dead code, predating this migration.** `setAutoESFRAW` (both overloads),
  `setAutoESFRAWrate`, `setAutoESFRAWcallbackPtr`, `assumeAutoESFRAW`, `logESFRAW`, and
  `initPacketUBXESFRAW` were all declared in `u-blox_GNSS.h` but had NO definitions anywhere in
  `u-blox_GNSS.cpp` (confirmed via grep before removing anything, same care taken for ESF-MEAS's
  smaller dead-declaration finding in Phase 30). Since `initPacketUBXESFRAW()` was never defined,
  `packetUBXESFRAW` could never actually be allocated - meaning every `if (packetUBXESFRAW !=
  nullptr)` branch that used to exist in the destructor, `autoLookup()`, `processUBXpacket()`, and
  `checkCallbacks()` was ALREADY unreachable dead code before this session touched any of it, not
  just after. Only `getRawSensorMeasurement()` (now redacted) and the raw parsing/storage/
  callback-firing code actually did anything, and even that could never fire in practice since the
  data it depended on was never allocated.
- **`u-blox_structs.h`**: removed only `UBX_ESF_RAW_t` and `UBX_ESF_STATUS_moduleQueried_t`/
  `UBX_ESF_STATUS_t` (the v3 RAM-management wrappers) - kept `UBX_ESF_RAW_data_t`/
  `UBX_ESF_RAW_sensorData_t`/`UBX_ESF_STATUS_data_t`/`UBX_ESF_STATUS_sensorStatus_t` (the
  wire-format structs) as documented reference, matching every prior migration.
  `UBX_ESF_RAW_MAX_LEN`/`UBX_ESF_STATUS_MAX_LEN` are unchanged and still used, as `messageLength`.
- **Verified statically:** confirmed every field's byte/bit offset in both new files against
  `UBX_ESF_RAW_sensorData_t`/`UBX_ESF_STATUS_data_t`/`UBX_ESF_STATUS_sensorStatus_t`'s actual
  struct layouts in `u-blox_structs.h` before writing the field tables, not after. Confirmed the
  `addClassID()` calls' argument order and count match the existing extended signature exactly (14
  positional arguments, matching MON-COMMS/SEC-SIG - neither new message uses the
  `blockCountField`/footer trailing parameters). Confirmed `ubxESFRAW.h`'s 3 block-field-table
  entries match its declared `numBlockFields`, and `ubxESFSTATUS.h`'s 4 header + 13 block entries
  match its declared `numFields`/`numBlockFields`. Confirmed
  `UBLOX_CFG_MSGOUT_UBX_ESF_{RAW,STATUS}_{I2C,SPI,UART1,UART2}` all exist in
  `u-blox_config_keys.h`, and that `UBX_ESF_RAW = 0x03`/`UBX_ESF_STATUS = 0x10` are already defined
  in `u-blox_Class_and_ID.h`. Confirmed brace/paren balance held on every touched file against the
  pre-edit baseline (`ubxMessage.h` 32/32 braces, 310/310 parens; `ubxMessageVector.h` 24/24,
  125/125; `u-blox_GNSS.h` 34/34, 889/889; `u-blox_structs.h` 292/292, 385/385; `ubxESFRAW.h` 7/7,
  46/46; `ubxESFSTATUS.h` 22/22, 53/53; `u-blox_GNSS.cpp` 1048/1048 braces, carrying the same
  pre-existing -2 paren imbalance documented since Phase 9, confirmed present at the pre-edit
  baseline too - not newly introduced). Confirmed CRLF preserved on every touched core file and
  both new files are LF-only, matching every other file in `ubxMessages/`. Grepped the whole `src/`
  tree afterward for every retired symbol (`packetUBXESFRAW`, `packetUBXESFSTATUS`,
  `UBX_ESF_RAW_t`, `UBX_ESF_STATUS_t`, `UBX_ESF_STATUS_moduleQueried_t`,
  `getRawSensorMeasurement`, `getSensorFusionStatus`, `setAutoESFRAW*`, `setAutoESFSTATUS*`,
  `assumeAutoESF*`, `logESFRAW`, `logESFSTATUS`, `initPacketUBXESFRAW`, `initPacketUBXESFSTATUS`)
  and confirmed every remaining hit is a comment, not live code.
- **Not yet done:** at the time this section was first written, neither message had been compiled
  with `compile_example.bat` (Docker has been unavailable in every sandbox tried this engagement)
  or hardware-validated. **Both gaps are now closed for both messages - see the two
  hardware-validation postscripts immediately below.** `numCallbackCopies = 1` for both was always
  a design decision (explicitly instructed), not an estimate to validate the way
  `UBX_ESF_MEAS_CALLBACK_BUFFERS`/`UBX_RXM_SFRBX_CALLBACK_BUFFERS` were, so there is no equivalent
  "may need raising" caveat to track for either message.

- **Hardware-validation postscript (ESF-RAW, added after the user tested `CallbackExample11_ESFRAW`
  on real hardware over I2C):** the user's own sketch (not written by Claude) registered a callback,
  printed `getUbxMessageBlockCountCallback()` as "Measurements: N", then looped over that count
  printing each block's decoded sensor value (X/Y/Z accelerometer, X/Y/Z gyro, temperature - `type`
  determined from `dataType` and `dataField` unshifted per sensor type in the sketch, mirroring the
  ESF-MEAS example) alongside `sTag` as "Sensor time". Result: compiled with no errors or warnings,
  and printed **"Measurements: 7"** on every burst, with all 7 sensor readings present, plausible,
  and self-consistent (accel ~[0.71, 1.67, 9.92] m/s^2 - a believable gravity-dominated static
  reading; gyro readings all near zero; temperature a stable ~31 deg C), and every reading within a
  given burst sharing the same `sTag` value, changing consistently burst-to-burst (6793763 ->
  6794009 -> 6794271 -> 6794529). This is the first real-hardware exercise of the actual-length-only
  `getBlockCount()` fallback added in this phase specifically for ESF-RAW (the one new piece of
  base-class logic this phase's work actually depended on, as opposed to just another field-table
  class) - since ESF-RAW has no block-count field to cross-check against, "7" printed correctly and
  consistently on every single burst is the only confirmation available that the fallback's
  `(actualLength - blockHeaderLength) / blockLength` arithmetic is correct, and it is. Also confirms
  the 24-bit `dataField` sub-field's `X4`/`U4` tagging (reused from SEC-SIG/ESF-MEAS) decoding
  correctly for a third message, and `sTag`'s plain 32-bit `U4` block field. **ESF-RAW is now
  considered hardware-validated.**

- **Hardware-validation postscript (ESF-STATUS, added after the user tested
  `CallbackExample12_ESFSTATUS` on real hardware):** the user's own sketch (not written by Claude)
  compiled with no errors or warnings, registered a callback, printed `iTOW` as "TOW", looked up
  `fusionMode` against a small label table ("0: Initialization mode"), read `numSens` directly
  (per the ordinary convention documented above - not via `getUbxMessageBlockCount()`) and printed
  it as "Sensors: N", then looped over that count printing each block's `type` sub-field as a
  sensor name (Gyro X/Y/Z, Accel X/Y/Z, Speed Ticks) alongside `ready` and `calibStatus`
  (label-mapped to "not calibrated") and the plain `freq` field. Result: **"Sensors: 7" on every
  burst**, with all 7 named correctly and in a stable order (Z Gyro, Speed Ticks, Y Gyro, X Gyro,
  X/Y/Z Accel), each with a plausible, per-sensor-correct `freq` (50 Hz for every IMU axis, 10 Hz
  for Speed Ticks - matching the ZED-F9R's typical configured output rates) and `ready = 1`
  throughout; `iTOW` advanced by exactly 1000 ms per burst (194474000 -> 194475000 -> 194476000),
  consistent with the sketch's polling cadence; `fusionMode` read as 0 ("Initialization mode")
  consistently, plausible for a receiver that had not yet completed sensor fusion calibration.
  This confirms: the 4 header fields (`iTOW`, `fusionMode`, `numSens` - `version` wasn't printed
  by the sketch but shares the same header-field extraction path); the per-block `type` sub-field
  (`sensStatus1`, bits 0-5) correctly distinguishing all 7 ZED-F9R IMU/wheel-tick sensor types; and
  the `ready` (`sensStatus1` bit 6) and `calibStatus` (`sensStatus2` bits 0-1) sub-fields decoding
  correctly alongside `type` from the *same* byte, confirming the multi-sub-field-per-byte bit
  extraction (already used by MON-COMMS's `txErrors` and SEC-SIG's `sigSecFlags`) works correctly
  for ESF-STATUS's denser packing too. The sketch didn't print `used` (`sensStatus1` bit 7),
  `timeStatus` (`sensStatus2` bits 2-3), or any of the four `faults` sub-fields
  (`badMeas`/`badTTag`/`missingMeas`/`noisyMeas`) - those remain formally unexercised by name, but
  they use the identical bit-extraction mechanism as `ready`/`calibStatus`, which did print
  correctly, so there is no reason to expect them to behave differently. **ESF-STATUS is now
  considered hardware-validated**, with that one narrow caveat. **UBX-ESF-STATUS and UBX-ESF-RAW
  are both hardware-validated as of this postscript - Phase 31 is complete.**

## Adding support for RXM-PMP

Please add RXM-PMP to `class` `ubxMessages`. You will see that the existing v3 code contains
`setRXMPMPcallbackPtr` and `setRXMPMPmessageCallbackPtr`. You do not need to provide the second
`message` method. Treat RXM-PMP like a standard variable-length UBX message. If the user wants to
write ("push") the complete message from a callback to another device, they can use the raw frame
accessors you added.

### Implemented

Implemented directly, no proposal - RXM-PMP's payload is the familiar header-plus-variable-length-
data shape already established by NAV-SAT/RXM-RAWX/RXM-MEASX/MON-COMMS/SEC-SIG/ESF-RAW, so nothing
here is architecturally novel. See `claude/v4-migration-status.md` (Phase 32) for the as-built
record. Summary of what shipped:

- `class ubxRXMPMP` (`src/ubxMessages/ubxRXMPMP.h`) - self-registered, following the same
  variable-length pattern as every message above. `numCallbackCopies = 1` (an ordinary single-slot
  "latest wins" message, like MON-COMMS/SEC-SIG - not a burst message like RXM-SFRBX/ESF-MEAS).
  PMP cannot be polled - it is "Output" only, same convention as ESF-RAW (no `getRXMPMP()`
  wrapper; the old v3 API never had one either).
- **Real design problem found while investigating, not mentioned in the request: RXM-PMP has TWO
  genuinely different wire layouts, selected by its `version` byte, and unlike SEC-SIG's Version 2
  vs Version 3 (which turned out to share an identical layout, Phase 29), PMP's two versions
  actually disagree on where fields sit.** Version 0x00 is FIXED length (528 bytes always):
  `userData` is a fixed 504 bytes starting right after the 20-byte common header (byte 20), and
  `fecBits`/`ebno` sit AFTER `userData`, at bytes 524/526. Version 0x01 is VARIABLE length (24 +
  `numBytesUserData`, up to 528): `fecBits`/`ebno`/a reserved byte sit right after the header
  (bytes 20/22/23), and `userData` is variable-length (0..504 bytes, per `numBytesUserData`),
  starting at byte 24. The old v3 code's own comment confirmed this asymmetry directly: "Note:
  length is variable with version 0x01" - i.e. NOT variable with version 0x00. The v4 field-table
  mechanism has one fixed byte offset per field, so it cannot represent "this field's position
  depends on a runtime byte value" - **this class therefore only correctly models Version 0x01's
  layout**, which is also the one actually matching the instruction to "treat RXM-PMP like a
  standard variable-length UBX message" (0x01 is the version that is actually variable-length). A
  real Version 0x00 message would be misparsed by this class - same category of accepted,
  deliberate limitation as SEC-SIG Version 1 being left unmodelled (Phase 28). **Nothing in this
  repo has confirmed which version a real NEO-D9S actually outputs** - per the Phase 29 SEC-SIG
  surprise (a receiver's real output can differ from what looks like the current documented
  version), this is worth checking against real hardware before trusting this class outright.
- `userData` is opaque payload data (PMP correction data bound for the D9S's downstream receiver),
  not a struct of named values, so it is modelled as 0..`UBX_RXM_PMP_MAX_USER_DATA` (504) repeated
  1-byte "blocks" (`blockLength = 1`) rather than a header/block/footer record layout.
  `numBytesUserData` is passed as `blockCountField`, so `getUbxMessageBlockCount()`/`...Callback()`
  give a defensively-clamped count (the ESF-MEAS-style pattern: minimum of the header field's own
  value and what actually fits in the received length) - not the ESF-RAW no-field-at-all fallback,
  since Version 0x01 does have a real, documented count field.
- **`setRXMPMPmessageCallbackPtr()` (the old v3 "push the whole message, including sync/checksum
  bytes, to another device" API) is NOT reimplemented, per the user's explicit instruction**: the
  generic raw-frame relay mechanism added for ESF-MEAS (Phase 30) -
  `getUbxMessageRawLengthCallback()`/`getUbxMessageRawPtrCallback()`, automatic for ANY message
  with a callback registered - already covers exactly that use case, for every message, not just
  PMP. `setRXMPMPcallbackPtr()` (the field-level callback) is retired too, replaced by the generic
  `setAutoCallbackPtr()` (by name "RXM"/"PMP"), same as every other migrated message.
- **Full migration of the old v3 scaffolding, same depth as MON-COMMS/SEC-SIG/ESF-RAW/ESF-STATUS:**
  removed `packetUBXRXMPMP`/`packetUBXRXMPMPmessage` (the two `UBX_RXM_PMP_t *`/
  `UBX_RXM_PMP_message_t *` members) and `initPacketUBXRXMPMP()`/`initPacketUBXRXMPMPmessage()`
  from `u-blox_GNSS.h`/`.cpp`; removed the destructor's cleanup blocks for both; removed the
  `autoLookup()`/`processUBXpacket()` branches for RXM-PMP under `case UBX_CLASS_RXM:` (the case
  label stays alive - UBX_RXM_QZSSL6/RXM_SFRBX/RXM_RAWX/RXM_MEASX still have code or comments
  there); removed the `checkCallbacks()` manual callback-firing blocks for both - each replaced
  with a short retiring comment pointing at this section. Removed `setRXMPMPcallbackPtr()` and
  `setRXMPMPmessageCallbackPtr()` entirely (declarations and definitions) - neither survives even
  as a thin wrapper, since the generic `setAutoCallbackPtr()` (by name "RXM"/"PMP") now does the
  same job, and the message-push use case is covered by the raw-frame relay accessors instead, per
  the instruction.
- **`u-blox_structs.h`**: removed only `UBX_RXM_PMP_t`/`UBX_RXM_PMP_message_t` (the v3
  RAM-management wrapper structs) - kept `UBX_RXM_PMP_data_t`/`UBX_RXM_PMP_message_data_t` (the
  wire-format structs) as documented reference, matching every prior migration, with a note on
  `UBX_RXM_PMP_data_t` recording the Version 0x00/0x01 layout difference and that only 0x01 is
  modelled.
- **Verified statically:** confirmed every field's byte offset in the new `ubxRXMPMP.h` against
  the real v3 `processUBXpacket()` extraction code (`extractInt(msg, 2)` for `numBytesUserData`,
  `extractLong(msg, 4/8/12)` for `timeTag`/`uniqueWord[0]`/`uniqueWord[1]`, `extractInt(msg, 16)`
  for `serviceIdentifier`, `extractByte(msg, 18/19)` for `spare`/`uniqueWordBitErrors`, the
  Version-0x01 branch's `extractInt(msg, 20)`/`extractByte(msg, 22)` for `fecBits`/`ebno`, and
  `userData` starting at byte 24) before trusting the field table - not just against
  `UBX_RXM_PMP_data_t`'s struct layout, since the struct itself doesn't distinguish the two
  versions' byte offsets the way the actual extraction code does. Confirmed `numFields` (10) and
  `numBlockFields` (1) match the field tables' literal entry counts. Confirmed the `addClassID()`
  call's argument order matches the existing extended signature exactly (15 positional arguments -
  through `blockCountField`, omitting the trailing footer parameters via their defaults, same
  shape as ESF-MEAS's call but without a footer). Confirmed
  `UBLOX_CFG_MSGOUT_UBX_RXM_PMP_{I2C,SPI,UART1,UART2}` all exist in `u-blox_config_keys.h`, and
  that `UBX_RXM_PMP = 0x72`/`UBX_CLASS_RXM = 0x02` are already defined in `u-blox_Class_and_ID.h`.
  Confirmed brace/paren balance held on every touched file against the pre-edit baseline
  (`ubxMessageVector.h` 24/24 braces, 125/125 parens; `u-blox_GNSS.h` 34/34 braces, 892/892 parens
  - up from 889/889, balanced; `u-blox_GNSS.cpp` 1023/1023 braces, down from 1048/1048 (blocks
  removed), 4607/4609 parens - the same pre-existing -2 paren quirk documented since Phase 9,
  confirmed unchanged, not newly introduced; `u-blox_structs.h` 290/290 braces, down from 292/292
  (two struct definitions removed), 387/387 parens; the new `ubxRXMPMP.h` 16/16 braces, 63/63
  parens, 8364 bytes). Confirmed the new file is LF-only, matching every other file in
  `ubxMessages/`, and confirmed CRLF preserved on every touched core file. Grepped the whole
  `src/` tree afterward for `UBX_RXM_PMP_t`/`UBX_RXM_PMP_message_t`/`setRXMPMPcallbackPtr`/
  `setRXMPMPmessageCallbackPtr`/`initPacketUBXRXMPMP`/`initPacketUBXRXMPMPmessage` and confirmed
  every remaining hit is a comment, not live code. Also grepped the `examples/` folder for the two
  retired functions and for `UBX_RXM_PMP` generally - no example references RXM-PMP at all, so
  nothing there needed updating or is at risk of failing to compile from this change.
- **Not yet done:** this implementation has NOT been compiled (Docker still unavailable in every
  sandbox tried so far) and has NOT been hardware-validated - no `CallbackExample`-style sketch
  exists yet for RXM-PMP, and testing it needs a NEO-D9S (a different, correction-data-focused
  module from the ZED-F9R/ZED-X20P used for every other phase's hardware validation so far), so
  this may need different hardware than what has validated everything up to Phase 31.
  `numCallbackCopies = 1` is an ordinary design choice, not an estimate, so there is no "may need
  raising" caveat to track. The Version 0x00/0x01 limitation above is the main thing worth
  confirming against real hardware: if a real NEO-D9S turns out to send Version 0x00 (or a mix of
  both), this class's field table will misparse those specific messages, the same way it would for
  any message whose real-world version doesn't match what was assumed.

## Adding support for RXM-QZSSL6

Please add RXM-QZSSL6 to `class` `ubxMessages`. You may need to refer to
https://content.u-blox.com/sites/default/files/u-blox-D9-QZS-1.01_InterfaceDescription_UBX-21031777.pdf
for the full message description. RXM-QZSSL6 is like RXM-PMP in that in can not be polled, only
made periodic. And, importantly, the QZSSL6 messages are output two at a time. Please implement
`const uint8_t numCallbackCopies = UBX_RXM_QZSSL6_NUM_CHANNELS;` where `UBX_RXM_QZSSL6_NUM_CHANNELS`
is 2.

### Implemented

Implemented directly, no proposal - like RXM-PMP (Phase 32), nothing here is architecturally
novel: QZSSL6's header-plus-opaque-payload shape is the familiar pattern already established by
NAV-SAT/RXM-RAWX/RXM-MEASX/MON-COMMS/SEC-SIG/ESF-RAW/RXM-PMP. The one genuinely new wrinkle is the
`numCallbackCopies = 2` ring buffer the user explicitly asked for - see below. See
`claude/v4-migration-status.md` (Phase 33) for the as-built record. Summary of what shipped:

- `class ubxRXMQZSSL6` (`src/ubxMessages/ubxRXMQZSSL6.h`) - self-registered, consulting the u-blox
  D9-QZS 1.01 Interface Description (UBX-21031777) for the exact field layout, cross-checked
  against the old v3 code's own `UBX_RXM_QZSSL6_data_t` struct (which agreed). QZSSL6 cannot be
  polled - it is "Output" only, same convention as RXM-PMP/ESF-RAW (no `getRXMQZSSL6()` wrapper;
  the old v3 API never had one either - it only ever offered a raw-message callback,
  `setRXMQZSSL6messageCallbackPtr`, no field-based one at all).
- **Unlike RXM-PMP, QZSSL6's payload is FIXED length, not variable** - a single, unconditional
  264-byte payload (a 14-byte header + a fixed 250-byte `msgBytes` array), with no byte-count field
  anywhere in the header. Matching ESF-RAW's precedent (Phase 31) for a message with no
  block-count field at all, `blockCountField` is left at its default `nullptr` -
  `getUbxMessageBlockCount()`/`...Callback()` fall back to the actual-length-derived count, which
  for this message always comes out to the full 250 (264 - 14 header bytes, / 1 byte per block),
  matching `maxBlocks` exactly since the length never actually varies. `msgBytes` itself (the raw
  QZSS L6 payload, whose own format is defined by a different specification, IS-QZSS-L6-001, not
  otherwise modelled by this repo) is opaque payload data, so - exactly like RXM-PMP's `userData` -
  it is modelled as 0..`UBX_RXM_QZSSL6_DATALEN` (250) repeated 1-byte "blocks" rather than a
  struct of named fields.
- **The instructed `numCallbackCopies = UBX_RXM_QZSSL6_NUM_CHANNELS` (2) is the one real design
  point in this phase, and it is a genuine departure from every other output-only status-like
  message registered so far (MON-COMMS/SEC-SIG/RXM-PMP/ESF-STATUS, all `numCallbackCopies = 1`).**
  QZSSL6 messages are output two at a time - one per L6 reception channel (Channel A / Channel B) -
  so a single callback slot would let the second message of a pair silently overwrite the first
  before `checkCallbacks()` gets a chance to drain it, the same problem RXM-SFRBX's/ESF-MEAS's
  ring buffers solve for a burst of many messages. QZSSL6 gets the identical ring-buffer machinery
  (generic since Phase 23/Phase 30), just with a ring of exactly 2 rather than a larger
  burst-sized number - and, unlike RXM-SFRBX's/ESF-MEAS's buffer counts, this is an exact
  instruction (there are always exactly 2 channels, per the Interface Description), not an
  estimate that might need raising after real traffic.
- **`chInfo` (a 2-byte header field) is decoded into four sub-fields per the Interface
  Description's bit breakdown**: `chn` (bits 9:8, receiver channel 0/1), `msgName` (bit 10,
  0=L6D/1=L6E), `errStatus` (bits 13:12, 0=unknown/1=error-free/2=erroneous), `chName` (bits 15:14,
  channel name 0=A/1=B). All four share `chInfo`'s own byte offset (10) as their `startByte`, with
  `startBit` set to their bit position within the 2-byte field (8/10/12/14) -
  `ubxMessage::extractBits()` already supports a `startBit` beyond the first byte, reading as many
  little-endian bytes as `startBit`+`bitWidth` spans (the same mechanism SEC-SIG's 24-bit
  `centFreq` (Phase 28) relies on, just spanning via a `startBit` offset here instead of a wide
  `bitWidth`) - this is the first message in the registry to actually exercise that part of the
  mechanism, though it was already implicitly proven correct by extension of the `centFreq` case.
  The raw `chInfo` value is also exposed as a plain field, for a caller that wants to decode it a
  different way.
- **Full migration of the old v3 scaffolding, same depth as RXM-PMP (Phase 32):** removed
  `packetUBXRXMQZSSL6message` (the `UBX_RXM_QZSSL6_message_t *` member) and
  `initPacketUBXRXMQZSSL6message()` from `u-blox_GNSS.h`/`.cpp`; removed the destructor's cleanup
  block; removed the `autoLookup()`/`processUBXpacket()` branches for QZSSL6 under
  `case UBX_CLASS_RXM:` (the case label stays alive - UBX_RXM_SFRBX/RXM_RAWX/RXM_MEASX are still
  handled there too, via comments pointing at the registry); removed the `checkCallbacks()` manual
  callback-firing block - each replaced with a short retiring comment pointing at this section.
  Removed `setRXMQZSSL6messageCallbackPtr()` entirely (declaration and definition) - it does not
  survive even as a thin wrapper, since the generic `setAutoCallbackPtr()` (by name
  "RXM"/"QZSSL6") now does the same job, and the message-push use case is covered by the raw-frame
  relay accessors instead, per the same reasoning already applied to RXM-PMP.
- **Incidental finding while removing the dead v3 scaffolding, not a bug introduced by this
  phase:** the old `processUBXpacket()` code carried two comments directly above its QZSSL6
  parsing block - "Note: length is variable with version 0x01" and "Note: the field positions
  depend on the version" - that describe RXM-PMP's version-dependent layout (Phase 32), not
  QZSSL6's. QZSSL6's payload has always been fixed-length with a single field layout; these
  appear to have been a copy-paste artifact from adjacent PMP-era v3 code. They are gone now,
  along with the rest of that dead block - noted here since they could otherwise have been
  mistaken for a real QZSSL6 versioning concern.
- **`u-blox_structs.h`**: removed only `ubxQZSSL6AutomaticFlags`/`UBX_RXM_QZSSL6_t` (the v3
  RAM-management wrapper struct) - kept `UBX_RXM_QZSSL6_data_t`/`UBX_RXM_QZSSL6_message_data_t`
  (the wire-format structs) as documented reference, matching every prior migration. Unlike
  RXM-PMP's struct, `UBX_RXM_QZSSL6_data_t` needed no version-layout caveat added, since QZSSL6 has
  only one layout.
- **Verified statically:** confirmed every field's byte offset in the new `ubxRXMQZSSL6.h` against
  the old v3 code's own `UBX_RXM_QZSSL6_data_t` struct layout in `u-blox_structs.h` (version@0,
  svId@1, cno@2, timeTag@4, groupDelay@8, bitErrCorr@9, chInfo@10, reserved0@12-13, msgBytes@14)
  and against the u-blox D9-QZS 1.01 Interface Description's own field table, which agreed.
  Confirmed `numFields` (11) and `numBlockFields` (1) match the field tables' literal entry
  counts. Confirmed the `addClassID()` call's argument order matches the existing extended
  signature exactly (the same 15-positional-argument shape as RXM-PMP's call). Confirmed
  `UBLOX_CFG_MSGOUT_UBX_RXM_QZSSL6_{I2C,SPI,UART1,UART2}` all exist in `u-blox_config_keys.h`
  (USB deliberately unused, per the repo-wide convention), and that
  `UBX_RXM_QZSSL6 = 0x73`/`UBX_CLASS_RXM = 0x02` are already defined in `u-blox_Class_and_ID.h`.
  Confirmed brace/paren balance held on every touched file against the pre-edit baseline
  (`ubxMessageVector.h` 24/24 braces, 126/126 parens; `u-blox_GNSS.h` 34/34 braces, 901/901
  parens - up from 892/892, balanced; `u-blox_GNSS.cpp` 1009/1009 braces, down from 1023/1023
  (blocks removed), 4583/4585 parens - the same pre-existing -2 paren quirk documented since
  Phase 9, confirmed unchanged, not newly introduced; `u-blox_structs.h` 286/286 braces, down from
  290/290 (one struct definition removed), 390/390 parens; the new `ubxRXMQZSSL6.h` 17/17 braces,
  66/66 parens, 8050 bytes). Confirmed the new file is LF-only, matching every other file in
  `ubxMessages/`, and confirmed CRLF preserved on every touched core file. Grepped the whole
  `src/` tree afterward for `packetUBXRXMQZSSL6message`/`ubxQZSSL6AutomaticFlags`/
  `UBX_RXM_QZSSL6_t`/`setRXMQZSSL6messageCallbackPtr`/`initPacketUBXRXMQZSSL6message` and
  confirmed every remaining hit is a comment, not live code. Also grepped the `examples/` folder
  for the retired function and for `UBX_RXM_QZSSL6`/`QZSSL6` generally - no example references
  RXM-QZSSL6 at all, so nothing there needed updating or is at risk of failing to compile from
  this change.
- **Not yet done:** this implementation has NOT been compiled (Docker still unavailable in every
  sandbox tried so far) and has NOT been hardware-validated - no `CallbackExample`-style sketch
  exists yet for RXM-QZSSL6, and testing it needs a NEO-D9C (a different, QZSS-L6-focused module
  from the ZED-F9R/ZED-X20P used for every other phase's hardware validation so far, and also
  different from RXM-PMP's NEO-D9S). The `numCallbackCopies = 2` ring buffer is the main thing
  worth confirming against real hardware: specifically, that both channels' messages really do
  arrive close enough together that a 2-slot ring (rather than a larger one) is genuinely
  sufficient, and that `checkCallbacks()` drains both before either could be overwritten by the
  next epoch's pair.

## Test

Compile the example code in examples/Example1\_PositionVelocityTime using the batch file compile\_example.bat.
Parse the Docker (Dockerfile) output. Check that the example compiles successfully.
