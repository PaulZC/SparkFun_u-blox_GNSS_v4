# Adding UBX-ESF-MEAS to `class ubxMessage` — Design Proposal

**Status:** Approved by the user on 2026-09-21. Implemented in the same session (see
`claude/v4-migration-status.md`, Phase 30, for the as-built record).

## 1. Why ESF-MEAS needs a revised strategy

ESF-MEAS combines three things separately:

1. **Variable-length, repeated blocks** — already supported by `ubxMessage`
   (`_blockFields`/`_numBlockFields`/`_blockHeaderLength`/`_blockLength`/`_maxBlocks`), used by
   NAV-SAT, NAV-SIG, RXM-RAWX, RXM-MEASX, MON-COMMS and SEC-SIG.
2. **Multiple ring-buffered callback copies** — already supported (`_numCallbackCopies > 1`), used
   by RXM-SFRBX.
3. **A block-count header field that cannot be trusted, and an optional trailing footer group
   whose presence cannot be inferred from the data itself.** This is new. No message registered so
   far has either problem.

From the current v3 parsing code (`u-blox_GNSS.cpp`, `processUBXpacket()`):

```cpp
for (i = 0; (i < DEF_MAX_NUM_ESF_MEAS) && (i < flags.bits.numMeas) && ((i * 4) < (msg->len - 8)); i++)
  ...
if (msg->len > 8 + numMeas * 4)
  calibTtag = extractLong(msg, 8 + numMeas * 4);
```

`numMeas`'s own field comment says it is "optional, can be obtained from message size" — i.e.
u-blox does not guarantee the receiver populates it correctly. The v3 code defensively
cross-checks it against the **actual received byte length** on every read, and uses that same
actual length — not a flag bit (`calibTtagValid` is "always set to zero") — to decide whether the
trailing `calibTtag` is present at all.

## 2. The gap this exposes: the registry doesn't remember how many bytes actually arrived

`ubxMessageVector::storePayload()` copies `len` bytes into `_storage` and stops:

```cpp
if (len > msg->_messageLength) len = msg->_messageLength;
memcpy(msg->_storage, payload, len);
```

It does **not** zero the remainder of `_storage`, and `initStorage()` only zeroes the buffer once,
on first allocation. So if one ESF-MEAS message arrives with 10 measurements + a footer (52 bytes)
and the next has 5 measurements with no footer (28 bytes), bytes 28–51 of `_storage` still hold
the *previous* message's stale data after the second `storePayload()` call.

This has never mattered before, because every other variable-length message's header count field
is trusted directly, so callers only ever read positions "before" where staleness could show up.
For ESF-MEAS the header field is *not* trustworthy, so a defensive "min of header field vs.
actual length" check needs the real received length explicitly — inferring presence from buffer
content (e.g. "is the footer non-zero?") is unsafe.

This also affects the callback ring buffer: ESF-MEAS needs multiple callback copies (like
RXM-SFRBX), and each buffered copy can be a *different* message with its own measurement count and
its own footer presence — so the actual length has to be tracked **per ring slot**, not once for
the live `_storage` copy.

## 3. Base-class additions (`ubxMessage`) — additive and opt-in; no change for any existing message

1. **`uint16_t _actualLength`** — the real payload byte length of the most recent `_storage`
   write, set by `storePayload()` from its already-available `len` parameter.
2. **`uint16_t *_callbackActualLength`** — heap array of `_numCallbackCopies` entries, allocated
   alongside `_callbackStorage` in `initCallbackStorage()`, written by `storePayload()`'s
   callback-write branches, read via the existing `_callbackReadIndex`. Degenerates to one entry
   for every message with `_numCallbackCopies <= 1`, exactly like `_callbackStorage` already does.
3. **`const char *_blockCountField`** (new optional `addClassID()` parameter, default `nullptr`) —
   names the header field that *nominally* holds the block count ("numMeas" for ESF-MEAS). When
   set, `ubxMessage::getBlockCount()` computes the count defensively:
   `min(headerFieldValue, (actualLength - blockHeaderLength) / blockLength, maxBlocks)`.
   Left `nullptr` for every message registered so far — unaffected.
4. **`_footerFields` / `_numFooterFields` / `_footerLength`** (new optional `addClassID()`
   parameters, mirroring `_blockFields` etc., default `nullptr`/`0`/`0`) — an optional trailing
   group after the last real block. Its start offset is computed dynamically from the defensive
   block count above (not `_maxBlocks`), and a read only succeeds if
   `actualLength >= footerOffset + footerLength` — otherwise the existing "field not found"
   sentinel is returned, so a caller can tell "no footer this time" from "footer is zero."

## 4. New capability, added at the user's request during review: relaying the raw UBX message

Beyond ESF-MEAS itself, the user asked for a general capability: from inside a callback, be able
to relay the **complete raw UBX message** (6-byte header + payload + 2-byte checksum) verbatim to
another device/UART, without reconstructing it by hand.

This reuses the actual-length tracking above and extends it:

- **`uint8_t *_callbackRawFrame`** — heap array of `_numCallbackCopies` slots, each
  `8 + _messageLength` bytes, allocated alongside `_callbackStorage`/`_callbackActualLength` in
  `initCallbackStorage()`. Each slot holds the complete synthesized frame: `0xB5, 0x62, Class, ID,
  lenLo, lenHi, payload[0..len-1], checksumA, checksumB`. `storePayload()` gains two new
  parameters, `checksumA`/`checksumB` (available at its one call site, `processUBXpacket()`,
  straight from the already-validated `ubxPacket`), and synthesizes this frame into the same ring
  slot as the payload-only `_callbackStorage` write, whenever a callback is registered.
- **`getUbxMessageRawLengthCallback(ubxMessage *theMessage)`** — returns the complete frame length
  (`8 + _callbackActualLength[_callbackReadIndex]`) for the message a callback just fired for, or
  `0` if unavailable.
- **`getUbxMessageRawPtrCallback(ubxMessage *theMessage)`** — returns a `const uint8_t *` to the
  start of that complete frame within `_callbackRawFrame`, or `nullptr` if unavailable. A user can
  pass this pointer and the length above straight to e.g. `Serial2.write(ptr, len)`.

This is implemented generically on `ubxMessage`/`ubxMessageVector` (not ESF-MEAS-specific) — any
registered message with a callback registered gets it automatically, the same way
`_callbackStorage` itself is automatic once a callback is registered, with no separate opt-in flag
to discover and set.

**RAM cost, worth the user's awareness:** this duplicates the payload bytes (they already exist in
`_callbackStorage`) to give one contiguous, ready-to-write buffer, rather than reconstructing the
frame on demand or restructuring `_callbackStorage`'s existing layout (which every field-extraction
call site across ~40 messages already depends on). The added cost is
`_numCallbackCopies * (8 + _messageLength)` bytes, on top of the existing
`_numCallbackCopies * _messageLength` bytes for `_callbackStorage` — i.e. roughly double the
existing callback-storage RAM cost for any message with a callback registered, allocated lazily
(only once a callback is actually registered, same as `_callbackStorage` itself).

## 5. New generic accessors (reusable by any future message with the same shape, not ESF-MEAS-only)

- `getUbxMessageBlockCount(ubxMessage *theMessage)` / `...Callback(...)` — the defensively-computed
  real block count, for a message that set `_blockCountField`.
- `getUbxMessageFooterField(ubxMessage *theMessage, const char *fieldName)` / `...Callback(...)` —
  mirrors `getUbxMessageBlockField()`, reading a named footer field; returns the "not found"
  sentinel if the footer was not actually present in that particular message.
- `getUbxMessageRawLengthCallback(ubxMessage *theMessage)` /
  `getUbxMessageRawPtrCallback(ubxMessage *theMessage)` — see §4.

## 6. `ubxESFMEAS.h` shape

- `messageLength = UBX_ESF_MEAS_MAX_LEN` (136 bytes — already sized for the max 31 blocks + 4-byte
  footer, unchanged).
- `numCallbackCopies = UBX_ESF_MEAS_CALLBACK_BUFFERS` (currently 6) — same ring mechanism already
  built for RXM-SFRBX. **Per the Phase 26 precedent with RXM-SFRBX (14 buffers assumed, 50 needed
  on real hardware — more than 3x), this should be treated as a starting point to validate on
  hardware once implemented, not a number to trust blindly.**
- Header (`ubxFields`, 8 bytes): `timeTag` (U4, byte 0), `flags` (X2, byte 4) with sub-bits
  `timeMarkSent` (bit 0, width 2), `timeMarkEdge` (bit 2, width 1), `calibTtagValid` (bit 3, width
  1 — present but not relied on for footer detection, per u-blox's "always zero" note), `numMeas`
  (bit 11, width 5), and `id` (U2, byte 6).
- Block (`ubxBlockFields`, 4 bytes, `blockCountField = "numMeas"`, `maxBlocks =
  DEF_MAX_NUM_ESF_MEAS` = 31): `dataField` (24-bit, bits 0–23 — same width as SEC-SIG's `centFreq`,
  so it needs the same `X4`/`U4` tagging as that field, not `U1`, per Phase 28's bitfield-tagging
  finding) and `dataType` (6-bit, bits 24–29).
- Footer (`ubxFooterFields`, 4 bytes): `calibTtag` (U4, footer-relative byte 0).
- `msgOutKeys[4] = {I2C, SPI, UART1, UART2}` — no USB key, matching the repo-wide convention.

## 7. Migration/cleanup

Same depth as MON-COMMS/SEC-SIG: remove `packetUBXESFMEAS`, `initPacketUBXESFMEAS()`,
`UBX_ESF_MEAS_t` (the v3 RAM-management wrapper), the destructor cleanup block, the manual
ring-buffer callback-firing block in `checkCallbacks()` (replaced by the same generic walk already
used for SFRBX/MON-COMMS/SEC-SIG), and the `setAutoESFMEAS*`/`assumeAutoESFMEAS`/`flushESFMEAS`/
`logESFMEAS` family (the generic `setAutoUBX`/etc. cover it). `getESFMEAS()` is kept as a one-line
thin wrapper, matching every other migrated message.

Unlike MON-COMMS/SEC-SIG, **`case UBX_CLASS_ESF:` stays alive** in both `autoLookup()` and
`processUBXpacket()`'s switches — ESF-RAW and ESF-STATUS still have real v3 code there — so only
the `if (ID == UBX_ESF_MEAS)` branch is removed, not the whole case.

`UBX_ESF_MEAS_data_t`/`UBX_ESF_MEAS_sensorData_t` (the wire-format structs) are kept as documented
reference, matching every prior migration.

## 8. Decisions made during review

- **`getSensorFusionMeasurement(UBX_ESF_MEAS_sensorData_t *sensorData, UBX_ESF_MEAS_data_t
  ubxDataStruct, uint8_t sensor)` is redacted entirely**, per the user's explicit instruction. It
  took its data struct by value from the caller, so it technically still compiled post-migration,
  but nothing constructs a live `UBX_ESF_MEAS_data_t` to pass it anymore once the v3 storage path
  is gone. No replacement helper is added — the generic `getUbxMessageBlockField()`/
  `getUbxMessageFooterField()` API covers the same ground.
- **The generalized base-class approach (§3, §5) is approved**, rather than a narrower
  ESF-MEAS-only hack — it sets up ESF-RAW/ESF-STATUS (the next two planned messages) to reuse the
  same defensive-block-count/footer machinery if they turn out to need it too.
- **Incidental finding, not part of this change:** the comment above `UBX_ESF_MEAS_MAX_LEN`
  currently reads "ESF RAW data cannot be polled. It is 'Output' only" — inside the ESF-MEAS
  section, so it looks like a copy-paste leftover meant for ESF-RAW. Corrected as part of this
  implementation since it sits directly above code being touched anyway.
