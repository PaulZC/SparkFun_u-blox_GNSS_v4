# Proposal: RXM-SFRBX in `ubxMessages`, with ring-buffered callback storage

## What I read first

`UBX_RXM_SFRBX_data_t`/`_message_data_t` and the current `processUBXpacket()`/`checkCallbacks()`
SFRBX blocks in `u-blox_GNSS.cpp`, plus `UBX_RXM_SFRBX_CALLBACK_BUFFERS` (14) in `u-blox_structs.h`.
SFRBX's payload shape is: an 8-byte header (`gnssId`, `svId`, `reserved1`, `freqId`, `numWords`,
`chn`, `version`, `reserved2`) followed by 0..16 repeated 4-byte `dwrd` words - the exact same
"header + repeated fixed-size blocks" shape as NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX. **That part
needs no new design** - it's a direct application of the Phase 19 mechanism.

The genuinely new part is what you've described: several SFRBX messages can arrive back-to-back
in a single `checkUblox()` call, and today's `_callbackStorage` (on `ubxMessage`) only holds one
slot - a second arrival overwrites the first before `checkCallbacks()` ever fires for it. That's
the actual design problem, and it applies to `ubxMessage`/`ubxMessageVector` generically, not to
SFRBX specifically (confirmed by your own sidenote - ESF-MEAS needs the same thing with 6 buffers).

One assumption I'm making: your new section refers throughout to `class nmeaMessage` /
`nmeaMessageVector`, but SFRBX is a UBX message, and `nmeaMessage`/`nmeaMessageVector` are a
separate, unrelated class hierarchy (their own `_callbackStorage`/`_callbackDataValid`, used only
by NMEA sentences). I'm reading this as referring to `ubxMessage`/`ubxMessageVector` throughout -
let me know if that's wrong, or if you actually want the NMEA side ring-buffered too (nothing in
the codebase currently needs that, since NMEA sentences arrive one at a time, not in bursts).

## Part 1: generic ring-buffered `_callbackStorage` (the core ask)

`_callbackStorage` is already allocated as `_messageLength * _numCallbackCopies` bytes (that
sizing has been in place since Phase 19, unused until now). What's missing is the bookkeeping for
*which* of the `_numCallbackCopies` slots is being written to or read from.

**New `ubxMessage` members** (alongside the existing `_numCallbackCopies`):
```cpp
uint8_t _callbackHead = 0;      // Next FREE slot to write into (storePayload)
uint8_t _callbackTail = 0;      // Next FRESH slot to read and dispatch (checkCallbacks)
uint8_t _callbackCount = 0;     // How many slots currently hold fresh, undelivered data
uint8_t _callbackReadIndex = 0; // Set transiently by checkCallbacks() before each callback firing - see Part 1c
```
I've added `_callbackCount` alongside the `_head`/`_tail` you suggested, rather than deriving
"how many buffers are fresh" from `head - tail` at check time - it's a one-line answer to exactly
the question your note raises ("make it easy to determine how many buffers contain fresh data"),
and it avoids wasting a slot to disambiguate "empty" from "full" (the classic ring-buffer gotcha
when you only have head/tail). Happy to drop it and do it the other way if you'd rather keep this
to exactly two new members.

`_callbackDataValid` (the current single-slot bool) is retired - `_callbackCount > 0` replaces it
everywhere. This only touches the **UBX** side of that name (`ubxMessage.h`, `ubxMessageVector.h`,
and the UBX half of `checkCallbacks()`); the separate NMEA `_callbackDataValid` is untouched.

**1a. `storePayload()` (`ubxMessageVector.h`) - the write side.**

This is the one place I want to flag a real behavior question. Today, *every* registered message
(all 30+ of them, `_numCallbackCopies == 1`) always **overwrites** the single callback slot with
the newest data - if the callback hasn't fired yet, the older value is simply superseded. That's
the right behavior for a "give me the latest" message like NAV-PVT.

Your note says the new ring buffer should write to `_head` "if space is available" - i.e., if it's
full, **drop the new message and keep what's already buffered**. That's the right behavior for
SFRBX/ESF-MEAS, where every individual message matters (an ephemeris/measurement decoder
downstream doesn't want a buffered-but-unread subframe silently replaced by a newer one) - but it's
the *opposite* choice from today's overwrite-always behavior, and I don't want to silently change
that for the 30+ messages already relying on it. So I'm proposing:

```cpp
if (msg->_callbackPtr != nullptr && msg->initCallbackStorage())
{
    if (msg->_numCallbackCopies <= 1)
    {
        // Unchanged from today: always overwrite the single slot (latest wins).
        memcpy(msg->_callbackStorage, payload, len);
        msg->_callbackCount = 1; // (head/tail stay at 0 - a degenerate 1-slot ring)
    }
    else if (msg->_callbackCount < msg->_numCallbackCopies) // Ring has a free slot
    {
        uint8_t *slot = msg->_callbackStorage + ((uint32_t)msg->_callbackHead * msg->_messageLength);
        memcpy(slot, payload, len);
        msg->_callbackHead = (msg->_callbackHead + 1) % msg->_numCallbackCopies;
        msg->_callbackCount++;
    }
    else
    {
        // Ring is full - drop this one and keep what's already buffered (per your note).
        // Worth a rate-limited debug print (mirrors the "RAM alloc failed!" style prints
        // elsewhere) so a sketch that's falling behind is discoverable, not just silently lossy.
    }
}
```
Every existing message keeps exactly today's behavior (verified by construction, not just by
argument: for `_numCallbackCopies == 1` the new code path *is* the old code path, just spelled
differently). Only SFRBX/ESF-MEAS (and anything else future that sets `numCallbackCopies > 1`) get
the new drop-when-full ring behavior.

**1b. `checkCallbacks()` (`u-blox_GNSS.cpp`) - the read side.**

The existing generic loop fires the callback once per message, if `_callbackDataValid`. It needs to
become a drain loop, since a single `checkCallbacks()` call may need to deliver several buffered
SFRBX messages, not just one:
```cpp
for (auto msg : ubxMessages.ubxMessageVectors)
{
    while ((msg->_callbackPtr != nullptr) && (msg->_callbackCount > 0))
    {
        msg->_callbackReadIndex = msg->_callbackTail; // Tell the getters which slot this firing reads - see 1c
        ubxCallbackDataCommon_t commonData;
        commonData.Class = msg->_Class;
        commonData.ID = msg->_ID;
        commonData.messagePtr = msg;
        msg->_callbackPtr(&commonData); // Call the callback
        msg->_callbackTail = (msg->_callbackTail + 1) % msg->_numCallbackCopies;
        msg->_callbackCount--;
    }
}
```
For `_numCallbackCopies == 1` this drains at most once per call, exactly as today.

**1c. `getUbxMessageFieldCallback()`/`getUbxMessageBlockFieldCallback()` - reading the right slot.**

These currently read from `theMessage->_callbackStorage` at an implicit offset of 0 (there's only
ever one slot today). With multiple buffered slots, they need to know *which* slot the callback
that's currently running was fired for.

I looked at two ways to do this:
- **(A)** Change their signature to take the `ubxCallbackDataCommon_t *theData` the callback
  actually received (adding a slot index to that struct), instead of the `ubxMessage *` a caller
  gets back from `getUbxMessagePtr(theData)`. This is the more "explicit" design, but it's a
  breaking change to a signature every existing callback example uses
  (`CallbackExample1` through `CallbackExample6`) - they'd all need a one-line edit.
- **(B)** Keep today's signatures unchanged, and instead have `checkCallbacks()` stash the slot
  index on the message object itself (`_callbackReadIndex`, above) immediately before firing the
  callback. The getters then compute `_callbackStorage + (_callbackReadIndex * _messageLength)` as
  their read base instead of `_callbackStorage + 0`. For `_numCallbackCopies == 1`,
  `_callbackReadIndex` is always 0, so every existing example keeps working unmodified.

I'm proposing **(B)** - zero-touch for every existing callback and example, and it's a small,
self-contained change (two functions, one new line each). `getUbxMessageBlockFieldCallback()` gets
the same offset added to its existing block-offset math.

## Part 2: `ubxRXMSFRBX.h` itself

Reuses the Phase 19 mechanism unchanged - no base-class work needed here beyond Part 1:
- Header (`ubxFields`, 6 entries): `gnssId`, `svId`, `freqId`, `numWords`, `chn`, `version`
  (`reserved1`/`reserved2` not exposed).
- Block (`ubxBlockFields`, 1 entry): `dwrd` (`U4`) - I'm not decomposing it further, since its bits
  mean different things depending on which GNSS/subframe it is; that decoding belongs to whatever
  consumes the raw words (e.g. an ephemeris parser), not to this field table.
- `blockHeaderLength = 8`, `blockLength = 4`, `maxBlocks = UBX_RXM_SFRBX_MAX_WORDS` (16).
- `numCallbackCopies = 14`, per your note.

## Part 3: the raw full-message callback - a real conflict, needs your decision

`setAutoRXMSFRBXmessageCallbackPtr()`/`UBX_RXM_SFRBX_message_data_t` is a **second, separate**
mechanism (nothing to do with parsed fields) that pushes the entire raw SFRBX packet - sync bytes,
checksum, everything - to a callback, for feeding the PointPerfect library. It isn't mentioned in
your new AGENTS.md section, but I need to flag a real interaction: once `ubxRXMSFRBX` is registered
in the generic registry, `processUBXpacket()`'s registry-first branch (`if (ubxMessagePtr) {
storePayload(...); }`) takes over *completely* for that Class/ID - the `else { switch(msg->cls)
{...} }` fallback that currently populates `callbackMessageData[]` would simply never run for
SFRBX again. So migrating SFRBX as described **would silently stop the raw-message callback from
ever firing**, even though nothing in this proposal touches its declaration.

Checked `examples/` - nothing currently uses `setAutoRXMSFRBXmessageCallbackPtr()`, so nothing
breaks today. But I don't want to leave a function that's still declared and compiles, but quietly
never fires, without you deciding that's what you want. Options:

1. **Retire it now**, alongside the rest of the legacy SFRBX API, with a comment pointing at it as
   future work (my default recommendation - it's a distinct mechanism, arguably deserves its own
   generic design the way the field-callback ring buffer just got, rather than being bolted on
   here; nothing depends on it yet).
2. **Fold it into this same pass** - give it the same ring-buffer treatment as the field callback
   (a second `_callbackStorage`-like buffer sized for the full raw packet, `_messageLength` being
   `UBX_RXM_SFRBX_MAX_LEN` plus the 8 header/checksum bytes). Doable, but roughly doubles the size
   of this change, and starts pulling in a second generic capability (raw-packet callbacks) that
   nothing has asked for elsewhere yet.
3. **Leave RXM-SFRBX out of the registry for now** - implement `ubxRXMSFRBX.h`'s field tables and
   the generic ring-buffer machinery (Part 1) so it's ready, but don't register it until the
   raw-message callback is also designed, so the current (if buffer-limited) legacy behavior
   doesn't regress in the meantime.

I'd default to (1) unless you tell me otherwise - it's the smallest change that satisfies what your
new AGENTS.md section actually asks for, and it doesn't leave anything silently broken.

## Legacy retirement scope (assuming option 1 above)

Same four-file pattern as NAV-SAT/NAV-SIG/RXM-RAWX/RXM-MEASX: `getRXMSFRBX()` becomes a one-line
`getUBX()` wrapper; `setAutoRXMSFRBX`/`setAutoRXMSFRBXrate`/`setAutoRXMSFRBXcallbackPtr`/
`setAutoRXMSFRBXmessageCallbackPtr`/`assumeAutoRXMSFRBX`/`flushRXMSFRBX`/`logRXMSFRBX` and
`packetUBXRXMSFRBX`/`initPacketUBXRXMSFRBX()` are all removed; `UBX_RXM_SFRBX_t` (and
`UBX_RXM_SFRBX_message_data_t`, if going with option 1) go the same way `UBX_NAV_SAT_t` etc. did -
removed, with the underlying `_data_t`/wire-format structs kept as documented reference.
`processUBXpacket()`'s SFRBX branch, its `checkCallbacks()` block, and the destructor/autoLookup()
entries are removed the same way RAWX/MEASX's were.

## What this sets up for ESF-MEAS

Once Part 1 lands, ESF-MEAS (your sidenote - 6 buffers) becomes exactly the same shape of change
NAV-SIG was after NAV-SAT: a new `ubxESFMEAS.h` with `numCallbackCopies = 6`, reusing everything in
Part 1 unchanged. Not part of this proposal, but worth confirming the design generalizes the way
you intended.

## Testing

Nothing I can compile-test myself (same as every phase so far). Once you're happy with this and I
build it, the thing actually worth exercising on hardware is the burst case itself - a callback
example that counts how many SFRBX messages arrive per `checkCallbacks()` call and confirms none
are silently dropped at a realistic message rate (and, separately, deliberately calling
`checkCallbacks()` rarely enough to prove the drop-when-full path also behaves as expected, rather
than corrupting anything).

---

No code changed yet - this is the proposal, per your instruction. Let me know about the
`nmeaMessage`/`ubxMessage` assumption, the `_callbackCount` addition, and (most importantly) which
way you want the raw-message-callback conflict in Part 3 handled, and I'll implement accordingly.
