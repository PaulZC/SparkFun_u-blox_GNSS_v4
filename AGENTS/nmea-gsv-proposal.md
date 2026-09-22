# Proposal: NMEA GSV support in `nmeaMessages`

## What I read first

`nmeaMessage.h` (the base class, `extractFieldFrom()`, `addNMEA()`), `nmeaMessageVector.h`
(`storePayload`/callback dispatch is actually inline in `u-blox_GNSS.cpp`'s NMEA `process()`
block, not in a `storePayload()` method the way the UBX side has one), `nmeaGGA.h` (the template
for a new per-message header), the NMEA checksum/dispatch block in `process()` (~line 1330-1400),
and `checkCallbacks()`'s NMEA loop (~line 3474). I also compared directly against how Phase 19
(variable-length UBX-NAV-SAT) and Phase 23 (RXM-SFRBX's ring-buffered `_callbackStorage`) solved
the two problems GSV needs *both* of at once, since AGENTS.md's own GSV section asks for the same
`_callbackReadIndex` technique as `ubxMessage`.

GSV is the first NMEA message that's genuinely different in shape from the other 8: it has a
**variable number of repeated blocks** (1-4 satellites per sentence) sitting **between** a header
and a footer, and the GNSS module emits **several GSV sentences back-to-back** (up to 9 per
constellation) within one navigation cycle - both properties the existing `nmeaMessage`
architecture doesn't have to deal with anywhere else.

## Part 1: locating fields around a variable-length block (new, no UBX equivalent)

UBX's variable-length messages (NAV-SAT, NAV-SIG, RXM-RAWX/MEASX/SFRBX) only ever have a header
then blocks - no footer - so `getUbxMessageBlockField()` can just compute a fixed byte offset
(`_blockHeaderLength + blockIndex * _blockLength`) into the binary payload and hand that straight
to `extractFieldFrom()`. GSV can't do that: it's ASCII, comma-delimited, and has a footer
(`signalId`) *after* the blocks, at a text position that depends on how many blocks are actually
present in that specific sentence (1-4, not always the max of 4).

Your AGENTS.md sketch already gives the field-numbering scheme I'll implement: `nmeaFields[]`
holds the header (`fieldNumber` 0-3) and footer (`fieldNumber` 4, i.e. `numHeaderFields`) as if
there were zero blocks; `nmeaBlockFields[]` holds the 4 block fields with their own 0-3 numbering
relative to one block. To actually find a field's real comma position in the sentence, I need:

- **Header field** (`fieldNumber < numHeaderFields`): real position = `fieldNumber`, same as every
  other message today.
- **Block field** (`blockIndex`, `fieldNumber` from `nmeaBlockFields[]`): real position =
  `numHeaderFields + (blockIndex * numBlockFields) + fieldNumber`.
- **Footer field** (`fieldNumber >= numHeaderFields`): real position depends on how many blocks
  *this sentence* actually has, which I get by first counting the sentence's total comma/`*`
  delimited fields (call it `T`), then `actualBlockCount = (T - numHeaderFields - numFooterFields)
  / numBlockFields`, then real position = `numHeaderFields + (actualBlockCount * numBlockFields) +
  (fieldNumber - numHeaderFields)`.

  Hand-checked against `$GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,184,17,08,54,298,19,1*66`:
  `T` = 21 fields (indices 0-20), `numHeaderFields` = 4, `numFooterFields` = 1, so
  `actualBlockCount` = (21-4-1)/4 = 4 (correct - 4 blocks are present), and `signalId`'s real
  position = 4 + 16 + 0 = 20, which is exactly where `"1"` sits before `*66`. ✓.

This only changes behavior when `_blockFields != nullptr` (i.e. only for GSV) - every other
message's `extractFieldFrom()` call is completely unaffected, same "additive, defaulted
parameters" approach Phase 19 used on the UBX side.

`getNmeaMessageBlockField()`/`getNmeaMessageBlockFieldCallback()` (new `DevUBLOXGNSS` members,
mirroring the UBX pair) will explicitly bounds-check `blockIndex >= maxNumBlocks` and return an
empty `String` before ever touching `extractFieldFrom()`, per your note - this is a deliberate
difference from the UBX block accessors, which don't bounds-check at all (their block length is
fixed, so an out-of-range block is merely a read of unused-but-allocated memory; GSV's blocks are
ASCII of unknown extent, so I'd rather fail fast). Separately, if a caller asks for a `blockIndex`
that's `< maxNumBlocks` but still beyond what *this particular sentence* actually contains (e.g.
block 3 of a 2-block final message in a group), the real-position math above will point past the
sentence's actual field count, and the existing "ran off the end of the string" check in
`extractFieldFrom()` already returns `false`/empty for that - so it's safe without needing a second
bounds check against the live sentence.

## Part 2: ring-buffered `_callbackStorage` (same mechanism as RXM-SFRBX, applied to NMEA)

Today, NMEA's callback copy is a single slot with one bool (`_callbackDataValid`) - the inline
code in `process()`'s NMEA dispatch block overwrites it every time a new sentence with the same
message ID arrives. With `numCallbackCopies = 54`, GSV needs the same ring buffer Phase 23 built
for `ubxMessage`/RXM-SFRBX: `_callbackHead`/`_callbackTail`/`_callbackCount`/`_callbackReadIndex`
added to `nmeaMessage` (replacing `_callbackDataValid`, exactly as `ubxMessage` replaced it),
`checkCallbacks()`'s NMEA loop changed from a single `if` to a draining `while
(_callbackPtr != nullptr && _callbackCount > 0)` loop (FIFO, oldest first), and the inline
write-side code in `process()` changed to write into `_callbackStorage + (_callbackHead *
_messageLength)`, advance `_callbackHead`, and increment `_callbackCount` - **dropping the new
sentence if the ring is already full** (`_callbackCount == _numCallbackCopies`), same
"protect what's already buffered over the newest arrival" choice Phase 23 made for SFRBX. For
every other NMEA message (`numCallbackCopies == 1`, i.e. everyone except GSV), this degenerates to
exactly today's single-slot overwrite behavior - nothing else changes.

One thing worth being explicit about since it's easy to miss: **only `_callbackStorage` becomes a
ring buffer - `_storage` (the plain polling copy used by `getNMEA("GSV")`) stays a single slot**,
same as it already is for every message and same as `ubxMessage`'s `_storage` for NAV-SAT/SFRBX/
etc. That means polling `getNMEA("GSV")` in a loop only ever sees whichever single GSV sentence
arrived most recently - it does **not** accumulate a full picture across a constellation's GSV
group the way the ring-buffered callback path does. If you want "all satellites from the last
complete GSV group" via polling rather than a callback, that's a separate, bigger feature (some
kind of per-constellation accumulator) - out of scope here unless you want to fold it in.

`extractFieldFrom()`'s block/footer math above depends only on the buffer it's given (`_storage`
or one `_callbackStorage` slot), so it works identically for both `getNmeaMessageBlockField()`
(live `_storage`) and `getNmeaMessageBlockFieldCallback()` (the frozen slot at
`_callbackReadIndex`) - no special-casing needed there.

## New file `src/nmeaMessages/nmeaGSV.h`

```cpp
class nmeaGSV : public nmeaMessage
{
public:
    const char msgId[4] = "GSV";
    const uint8_t numCallbackCopies = 54; // 6 constellations * up to 9 messages per constellation
    const uint8_t messageLength = NMEA_GSV_MAX_LENGTH; // New constant, u-blox_structs.h - proposing 100 (same headroom as GNS/GST/RMC)

    static const uint8_t numHeaderFields = 4;
    static const uint8_t numBlockFields = 4;
    static const uint8_t maxNumBlocks = 4;
    static const uint8_t numFields = 5; // Header (4) + footer (1) - excludes numBlockFields, per AGENTS.md

    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_NMEA_ID_GSV_I2C, UBLOX_CFG_MSGOUT_NMEA_ID_GSV_SPI,
                                    UBLOX_CFG_MSGOUT_NMEA_ID_GSV_UART1, UBLOX_CFG_MSGOUT_NMEA_ID_GSV_UART2};

    const nmeaField nmeaFields[numFields] = {
        {"xxGSV", nmeaDataTypeString, 0},
        {"numMsg", nmeaDataTypeDigit, 1},
        {"msgNum", nmeaDataTypeDigit, 2},
        {"numSV", nmeaDataTypeNumeric, 3},
        {"signalId", nmeaDataTypeString, 4}, // Hex - kept as String, not Numeric
    };

    const nmeaField nmeaBlockFields[numBlockFields] = {
        {"svid", nmeaDataTypeNumeric, 0},
        {"elv", nmeaDataTypeNumeric, 1},
        {"az", nmeaDataTypeNumeric, 2},
        {"cno", nmeaDataTypeNumeric, 3},
    };

    nmeaGSV(void)
    {
        addNMEA(msgId, messageLength, numCallbackCopies, numFields,
                (const void *)&nmeaFields, (const uint32_t *)msgOutKeys,
                (const void *)&nmeaBlockFields, numBlockFields, numHeaderFields, maxNumBlocks);
    }
};

nmeaRegisterMessage(nmeaGSV);
```

## Things I'm deciding rather than asking about (flag if you'd rather I do it differently)

1. **`NMEA_GSV_MAX_LENGTH = 100`.** Worst case (`$GPGSV,9,9,99,` header + 4×`99,90,359,99,` blocks
   + `99*hh` footer) is ~80 chars; 100 matches the headroom convention already used for
   GNS/GST/RMC (also multi-field messages).
2. **Ring-buffer full policy: drop the newest, keep what's buffered** - same choice already made
   for RXM-SFRBX (Phase 23), for the same reason (a downstream consumer decoding satellite data
   doesn't want an unread sentence silently replaced).
3. **`getNmeaMessageBlockField()`/`...Callback()` added as new `DevUBLOXGNSS` members**, same
   pattern as `getNmeaMessageField()`/`...Callback()`, not as `nmeaMessage` methods.
4. Will add `getNmeaMessageBlockField`/`getNmeaMessageBlockFieldCallback` to `keywords.txt` next
   to the existing `getNmeaMessageField`/`...Callback` entries, same as Phase 19 did for the UBX
   block accessors.
5. No example `.ino` yet - I'll leave that for you to add and hardware-test, same as every other
   phase (no compiler available here).

## Testing

Compile `CallbackExample1_NAVHPPOSLLH` (or similar) to confirm the tree as a whole still builds
with `nmeaGSV.h` included. Real validation of the footer/block-index math and the ring buffer's
drain order needs a `CallbackExample`-style GSV sketch against live hardware, same as NAV-SAT/
RAWX/SFRBX were validated - worth specifically checking a burst of >1 constellation's worth of GSV
sentences arrives correctly ordered (FIFO) within one `checkCallbacks()` call.
