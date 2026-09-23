/**
 * @date 2026
 * @copyright Copyright (c) 2026, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 * 
 * Please see LICENSE.md for more details
 * 
 * An Arduino Library which allows you to communicate seamlessly with u-blox GNSS modules using the Configuration Interface
 * 
 * SparkFun sells these at its website: www.sparkfun.com
 * Do you like this library? Help support SparkFun. Buy a board!
 * https://www.sparkfun.com/sparkfun-allband-gnss-rtk-breakout-zed-x20p-qwiic.html
 * https://www.sparkfun.com/sparkfun-gps-rtk2-board-zed-f9p-qwiic-gps-15136.html
 * https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html
 * https://www.sparkfun.com/sparkfun-gnss-receiver-breakout-max-m10s-qwiic.html
 * https://www.sparkfun.com/sparkfun-gps-rtk-dead-reckoning-breakout-zed-f9r-qwiic-gps-22693.html
 *
 * Original version by Nathan Seidle @ SparkFun Electronics, September 6th, 2018
 * v2.0 rework by Paul Clark @ SparkFun Electronics, December 31st, 2020
 * v3.0 rework by Paul Clark @ SparkFun Electronics, December 8th, 2022
 * v4.0 rework by Claude, directed by Paul Clark @ SparkFun Electronics, September 2026
 *
 * https://github.com/sparkfun/SparkFun_u-blox_GNSS_v4
 *
 * 
 * @file u-blox_structs.h
 * 
 */

#pragma once

// Additional flags and pointers that need to be stored with each message type
struct ubxAutomaticFlags
{
  union
  {
    uint8_t all;
    struct
    {
      uint8_t automatic : 1;         // Will this message be delivered and parsed "automatically" (without polling)
      uint8_t implicitUpdate : 1;    // Is the update triggered by accessing stale data (=true) or by a call to checkUblox (=false)
      uint8_t addToFileBuffer : 1;   // Should the raw UBX data be added to the file buffer?
      uint8_t callbackCopyValid : 1; // Is the copy of the data struct used by the callback valid/fresh?
    } bits;
  } flags;
};

// NAV-specific structs

// UBX-NAV-POSECEF (0x01 0x01): Position solution in ECEF
const uint16_t UBX_NAV_POSECEF_LEN = 20;

// UBX-NAV-POSLLH (0x01 0x02): Geodetic position solution
const uint16_t UBX_NAV_POSLLH_LEN = 28;

// UBX-NAV-STATUS (0x01 0x03): Receiver navigation status
const uint16_t UBX_NAV_STATUS_LEN = 16;

// UBX-NAV-DOP (0x01 0x04): Dilution of precision
const uint16_t UBX_NAV_DOP_LEN = 18;

// UBX-NAV-ATT (0x01 0x05): Attitude solution
const uint16_t UBX_NAV_ATT_LEN = 32;

// UBX-NAV-PVT (0x01 0x07): Navigation position velocity time solution
const uint16_t UBX_NAV_PVT_LEN = 92;

// UBX-NAV-ODO (0x01 0x09): Odometer solution
const uint16_t UBX_NAV_ODO_LEN = 20;

// UBX-NAV-VELECEF (0x01 0x11): Velocity solution in ECEF
const uint16_t UBX_NAV_VELECEF_LEN = 20;

// UBX-NAV-VELNED (0x01 0x12): Velocity solution in NED frame
const uint16_t UBX_NAV_VELNED_LEN = 36;

// UBX-NAV-HPPOSECEF (0x01 0x13): High precision position solution in ECEF
const uint16_t UBX_NAV_HPPOSECEF_LEN = 28;

// UBX-NAV-HPPOSLLH (0x01 0x14): High precision geodetic position solution
const uint16_t UBX_NAV_HPPOSLLH_LEN = 36;

// UBX-NAV-PVAT (0x01 0x17): Navigation position velocity attitude time solution
const uint16_t UBX_NAV_PVAT_LEN = 116;

// UBX-NAV-TIMEUTC (0x01 0x21): UTC time solution
const uint16_t UBX_NAV_TIMEUTC_LEN = 20;

// UBX-NAV-CLOCK (0x01 0x22): Clock solution
const uint16_t UBX_NAV_CLOCK_LEN = 20;

// UBX-NAV-TIMELS (0x01 0x26): Leap second event information
const uint16_t UBX_NAV_TIMELS_LEN = 24;

// UBX-NAV-SAT (0x01 0x35): Satellite Information
// Typical X20P NAV-SAT messages contain 50-60 blocks
// For v4.0.0: settting UBX_NAV_SAT_MAX_BLOCKS to 128. 255 is too many, wasting RAM.
const uint16_t UBX_NAV_SAT_MAX_BLOCKS = 128; // numSvs is 8-bit
const uint16_t UBX_NAV_SAT_MAX_LEN = 8 + (12 * UBX_NAV_SAT_MAX_BLOCKS);

// UBX_NAV_SAT_t (the automaticFlags/moduleQueried/callbackPointerPtr/callbackData wrapper) and
// UBX_NAV_SAT_data_t/_header_t/_block_t (the wire-format reference structs) have all been removed -
// UBX-NAV-SAT is now a registered v4 message (ubxNAVSAT in src/ubxMessages/ubxNAVSAT.h), which
// carries the same bookkeeping generically via ubxMessage's own members, and whose field tables are
// the wire format's documentation now. See AGENTS.md "Adding the variable-length UBX messages" and
// "Tidying up u-blox_structs.h".

// UBX-NAV-SIG (0x01 0x43): Signal information
// Note: length is variable
// The ZED-X20P NAV-SIG messages typically contain 140 blocks. Setting this to 192 for v4.0.0
const uint8_t UBX_NAV_SIG_MAX_BLOCKS = 192;
const uint16_t UBX_NAV_SIG_MAX_LEN = 8 + (16 * UBX_NAV_SIG_MAX_BLOCKS);

// UBX_NAV_SIG_t (the automaticFlags/moduleQueried/callbackPointerPtr/callbackData wrapper) and
// UBX_NAV_SIG_data_t/_header_t/_block_t (the wire-format reference structs) have all been removed -
// UBX-NAV-SIG is now a registered v4 message (ubxNAVSIG in src/ubxMessages/ubxNAVSIG.h), which
// carries the same bookkeeping generically via ubxMessage's own members, and whose field tables are
// the wire format's documentation now. See AGENTS.md "Adding the variable-length UBX messages" and
// "Tidying up u-blox_structs.h".

// UBX-NAV-SVIN (0x01 0x3B): Survey-in data
const uint16_t UBX_NAV_SVIN_LEN = 40;

// UBX-NAV-RELPOSNED (0x01 0x3C): Relative positioning information in NED frame
// Note:
//  RELPOSNED on the M8 is only 40 bytes long
//  RELPOSNED on the F9 is 64 bytes long and contains much more information
const uint16_t UBX_NAV_RELPOSNED_LEN = 40;
const uint16_t UBX_NAV_RELPOSNED_LEN_F9 = 64;

// UBX-NAV-DAHEADING (0x01 0x45) Relative positioning information in the NED frame between antenna 1 and antenna 2 of the module
// Version 0x01 is 64 bytes and uses cm + 0.1mm
// Version 0x02 is 60 bytes and uses mm
const uint16_t UBX_NAV_DAHEADING_MAX_LEN = 64;

// UBX-NAV-AOPSTATUS (0x01 0x60): AssistNow Autonomous status
const uint16_t UBX_NAV_AOPSTATUS_LEN = 16;

// UBX-NAV-EOE (0x01 0x61): End of Epoch
const uint16_t UBX_NAV_EOE_LEN = 4;

// RXM-specific structs

// UBX-RXM-SFRBX (0x02 0x13): Broadcast navigation data subframe
// Note: SFRBX is output-only. Polling is not possible
// Note: length is variable
// Note: on protocol version 17: numWords is (0..16)
//       on protocol version 18+: numWords is (0..10)
// Note: the X20P has been observed exceeding both of the above - see UBX_RXM_SFRBX_MAX_WORDS.
// X20P needs more than 40 buffers!
// ZED-F9P @ 20Hz on SPI needs more than 50 buffers! If you have RAM to spare, set to 60 for best results.
const uint8_t UBX_RXM_SFRBX_CALLBACK_BUFFERS = 50;
const uint8_t UBX_RXM_SFRBX_MAX_WORDS = 20; // X20P occasionally outputs 19 words! (Payload length 84)
const uint16_t UBX_RXM_SFRBX_MAX_LEN = 8 + (4 * UBX_RXM_SFRBX_MAX_WORDS);

// UBX_RXM_SFRBX_message_data_t (the whole raw packet, for the retired PointPerfect push feature),
// ubxSFRBXAutomaticFlags, UBX_RXM_SFRBX_t (the v3 RAM-management wrapper) and UBX_RXM_SFRBX_data_t
// (the wire-format reference struct) have all been removed - UBX-RXM-SFRBX is now a registered v4
// message (ubxRXMSFRBX, in ubxMessages/ubxRXMSFRBX.h) with its own ring-buffered callback storage
// and field table, which is the wire format's documentation now. UBX_RXM_SFRBX_MAX_WORDS/
// UBX_RXM_SFRBX_MAX_LEN/UBX_RXM_SFRBX_CALLBACK_BUFFERS (above) are still used there and are kept as
// named, easily-tunable constants. See AGENTS.md "Adding support for RXM-SFRBX" and "Tidying up
// u-blox_structs.h".

// UBX-RXM-MEASX (0x02 0x14): Receiver Manager Messages: i.e. Satellite Status, RTC Status.
const uint8_t UBX_RXM_MEASX_MAX_BLOCKS = 92;
const uint16_t UBX_RXM_MEASX_MAX_LEN = 44 + (24 * UBX_RXM_MEASX_MAX_BLOCKS);

// UBX_RXM_MEASX_t (the automaticFlags/moduleQueried/callbackPointerPtr/callbackData wrapper) and
// UBX_RXM_MEASX_data_t/_header_t/_block_t (the wire-format reference structs) have all been
// removed - UBX-RXM-MEASX is now a registered v4 message (ubxRXMMEASX in
// src/ubxMessages/ubxRXMMEASX.h), which carries the same bookkeeping generically via ubxMessage's
// own members, and whose field tables are the wire format's documentation now. See AGENTS.md
// "Adding the variable-length UBX messages" and "Tidying up u-blox_structs.h".

// UBX-RXM-RAWX (0x02 0x15): Multi-GNSS raw measurement data
// Note: length is variable
// Up to v3.1.14, UBX_RXM_RAWX_MAX_BLOCKS was 92. This was set in v2 of the library (Oct 20 2021)
// As documented in issue #98, 92 is no longer adequate. X20P RAWX messages can contain 122+ blocks
// Increasing this to 140 for v4.0.0. Increases the memory usage...
const uint8_t UBX_RXM_RAWX_MAX_BLOCKS = 140;
const uint16_t UBX_RXM_RAWX_MAX_LEN = 16 + (32 * UBX_RXM_RAWX_MAX_BLOCKS);

// UBX_RXM_RAWX_t (the automaticFlags/moduleQueried/callbackPointerPtr/callbackData wrapper) and
// UBX_RXM_RAWX_data_t/_header_t/_block_t (the wire-format reference structs) have all been
// removed - UBX-RXM-RAWX is now a registered v4 message (ubxRXMRAWX in
// src/ubxMessages/ubxRXMRAWX.h), which carries the same bookkeeping generically via ubxMessage's
// own members, and whose field tables are the wire format's documentation now. See AGENTS.md
// "Adding the variable-length UBX messages" and "Tidying up u-blox_structs.h".

// UBX-RXM-COR (0x02 0x34): Differential correction input status
const uint16_t UBX_RXM_COR_LEN = 12;

// The COR data can only be accessed via a callback. COR cannot be polled.

// UBX-RXM-PMP (0x02 0x72): PMP raw data (D9 modules)
// There are two versions of this message but, fortunately, both have a max len of 528
const uint16_t UBX_RXM_PMP_MAX_USER_DATA = 504;
const uint16_t UBX_RXM_PMP_MAX_LEN = UBX_RXM_PMP_MAX_USER_DATA + 24;

// UBX_RXM_PMP_t/UBX_RXM_PMP_message_t (the v3 RAM-management wrapper structs - automaticFlags +
// callbackPointerPtr + callbackData) and UBX_RXM_PMP_data_t/UBX_RXM_PMP_message_data_t (the
// wire-format reference structs) have all been removed - ubxRXMPMP (src/ubxMessages/ubxRXMPMP.h)
// is now self-registered, and its field tables are the wire format's documentation now - see
// AGENTS.md "Adding support for RXM-PMP" and "Tidying up u-blox_structs.h". NOTE: ubxRXMPMP only
// correctly models Version 0x01's byte layout, not Version 0x00's (they genuinely disagree on
// field placement, unlike SEC-SIG's Version 2/3) - see the deliberate-limitation note in
// ubxRXMPMP.h. The raw-frame relay mechanism added for ESF-MEAS
// (getUbxMessageRawLengthCallback()/getUbxMessageRawPtrCallback()) covers the old "push the whole
// message" use case generically, for any registered message with a callback.

// UBX-RXM-QZSSL6 (0x02 0x73): QZSS L6 raw data (D9C modules). ubxRXMQZSSL6
// (src/ubxMessages/ubxRXMQZSSL6.h) is now self-registered - see AGENTS.md "Adding support for
// RXM-QZSSL6". UBX_RXM_QZSSL6_NUM_CHANNELS/_DATALEN/_MAX_LEN below are still used (as
// numCallbackCopies/the block table's maxBlocks/messageLength respectively).
const uint16_t UBX_RXM_QZSSL6_NUM_CHANNELS = 2;
const uint16_t UBX_RXM_QZSSL6_DATALEN = 250;
const uint16_t UBX_RXM_QZSSL6_MAX_LEN = UBX_RXM_QZSSL6_DATALEN + 14;

// UBX_RXM_QZSSL6_data_t/UBX_RXM_QZSSL6_message_data_t (the wire-format reference structs),
// ubxQZSSL6AutomaticFlags and UBX_RXM_QZSSL6_t (the v3 RAM-management wrapper - automaticFlags +
// callbackPointerPtr + callbackData) have all been removed - ubxRXMQZSSL6 is now self-registered
// (src/ubxMessages/ubxRXMQZSSL6.h), using the generic ring-buffered _callbackStorage/
// _callbackRawFrame mechanism (with numCallbackCopies = UBX_RXM_QZSSL6_NUM_CHANNELS, i.e. a
// 2-slot ring, one per L6 reception channel), and its field table is the wire format's
// documentation now. Unlike RXM-PMP, QZSSL6's payload has a single, fixed layout (no
// version-dependent field placement to worry about). See AGENTS.md "Adding support for
// RXM-QZSSL6" and "Tidying up u-blox_structs.h".

// CFG-specific structs - deleted at v3.0

// UBX-CFG-PRT (0x06 0x00): Port configuration
// UBX-CFG-RATE (0x06 0x08): Navigation/measurement rate settings
// UBX-CFG-TP5 (0x06 0x31): Time pulse parameters
// UBX-CFG-ITFM (0x06 0x39): Jamming/interference monitor configuration
// UBX-CFG-TMODE3 (0x06 0x71): Time Mode Settings 3

// MON-specific structs

// UBX-MON-COMMS (0x0A 0x36): Communication port information
// Note: length is variable
const uint16_t UBX_MON_COMMS_MAX_PORTS = 7; // I2C, UART1, UART2, USB, SPI, 0x0101, 0x0200
const uint16_t UBX_MON_COMMS_MAX_LEN = 8 + (40 * UBX_MON_COMMS_MAX_PORTS);

// UBX_MON_COMMS_t (the v3 RAM-management wrapper) and UBX_MON_COMMS_header_t/_port_t/_data_t
// (the wire-format reference structs) have all been removed - UBX-MON-COMMS is now a registered
// v4 message (ubxMONCOMMS, in ubxMessages/ubxMONCOMMS.h), and its field table is the wire
// format's documentation now. See AGENTS.md "Adding the variable-length UBX messages" and
// "Tidying up u-blox_structs.h".

// UBX-MON-HW (0x0A 0x09): Hardware status
const uint16_t UBX_MON_HW_LEN = 60;

// UBX-MON-HW2 (0x0A 0x0B): Extended hardware status
const uint16_t UBX_MON_HW2_LEN = 28;

// UBX-MON-RF (0x0a 0x38): RF information
const uint16_t UBX_MON_RF_MAX_BLOCKS = 3; // "The band which the RF block represents is subject to product configuration."
const uint16_t UBX_MON_RF_MAX_LEN = 4 + (24 * UBX_MON_RF_MAX_BLOCKS);

// UBX_MON_RF_header_t/_block_t/_data_t (the wire-format reference structs) have all been
// removed - UBX-MON-RF is now a registered v4 message (ubxMONRF, in ubxMessages/ubxMONRF.h),
// which carries the same bookkeeping generically via ubxMessage's own members, and whose field
// table is the wire format's documentation now. getRFinformation() has been replaced by
// getMONRF(), following the same pattern as MON-COMMS. See AGENTS.md "Adding the variable-length
// UBX messages" and "Tidying up u-blox_structs.h".

// TIM-specific structs

// UBX-TIM-TM2 (0x0D 0x03): Time mark data
const uint16_t UBX_TIM_TM2_LEN = 28;

// UBX-TIM-TP (0x0D 0x01): Time pulse time data
// Contains the Time-Pulse-Of-Week for the _next_ time pulse
// (PVT provides the time of the navigation solution which is usually
// aligned with the _previous_ pulse)
const uint16_t UBX_TIM_TP_LEN = 16;

// SEC-specific structs

// UBX-SEC-SIG (0x27 0x09): Signal security information
// Note: there are multiple versions of UBX-SEC-SIG.
//   Version 1 is 12 bytes, a different fixed layout (see versions.version1 below).
//   Version 2 is variable - a 4-byte header + repeated 4-byte center-frequency blocks (see
//   versions.version2 below); older interface descriptions document up to 6 frequencies.
//   Version 3 is ALSO variable, with the SAME byte layout as Version 2 (versions.version2 below
//   applies to it unchanged) - documented in the u-blox X20-HPG-2.11 interface description
//   (UBXDOC-304424225-21617). The ZED-X20P outputs Version 3, not Version 2: hardware-validated
//   (CallbackExample9_SECSIG) sending 7 center frequencies with no jamming present, so
//   UBX_SEC_SEG_MAX_CENT_FREQ_VERSION3 below is set to 10 for headroom, not a documented hard
//   maximum.
//   Check the version byte before attempting to read data from the struct.
const uint16_t UBX_SEC_SEG_MAX_CENT_FREQ_VERSION3 = 10; // ZED-X20P seems to default to 7 frequencies - without jamming
const uint16_t UBX_SEC_SIG_MAX_LEN_VERSION3 = 4 + (4 * UBX_SEC_SEG_MAX_CENT_FREQ_VERSION3);

// UBX_SEC_SIG_t (the v3 RAM-management wrapper) and UBX_SEC_SIG_data_t (the wire-format
// reference struct, both versions' layouts - previously kept here in commented-out form) have all
// been removed - UBX-SEC-SIG is now a registered v4 message (ubxSECSIG, in
// ubxMessages/ubxSECSIG.h), and its field table is the wire format's documentation now. ubxSECSIG
// models Version 3 (the version the ZED-X20P actually sends) - Version 1's different, fixed
// 12-byte layout is not modelled anywhere in the registry, per the user's original instruction.
// See AGENTS.md "Adding the variable-length UBX messages" and "Tidying up u-blox_structs.h".

// UBX-SEC-UNIQID (0x27 0x03): Unique chip ID
// The ID is five bytes on the F9 and M9 (version 1) but six bytes on the M10 (version 2)
const uint16_t UBX_SEC_UNIQID_LEN_VERSION1 = 9;
const uint16_t UBX_SEC_UNIQID_LEN_VERSION2 = 10;

// UBX_SEC_UNIQID_data_t (the wire-format reference struct) has been removed - UBX-SEC-UNIQID is
// now a registered v4 message (ubxSECUNIQID, in ubxMessages/ubxSECUNIQID.h). Its uniqueId bytes
// are modelled as repeated 1-byte "blocks" rather than a fixed 6-byte array, so a version-1
// module's 5 real bytes vs. a version-2 module's 6 don't need a version-dependent struct layout -
// see ubxSECUNIQID.h. getUniqueChipId()/getUniqueChipIdStr() have been replaced by
// getSECUNIQID()/getUniqueChipIdStr() (the latter now returns a hex String). See AGENTS.md
// "Adding the variable-length UBX messages" and "Tidying up u-blox_structs.h".

// ESF-specific structs

// UBX-ESF-ALG (0x10 0x14): IMU alignment information
const uint16_t UBX_ESF_ALG_LEN = 16;

// UBX-ESF-INS (0x10 0x15): Vehicle dynamics information
const uint16_t UBX_ESF_INS_LEN = 36;

// UBX-ESF-MEAS (0x10 0x02): External sensor fusion measurements
// Note: length is variable
// Note: ESF-MEAS is now implemented as its own self-registered Class - see ubxMessages/ubxESFMEAS.h
// and AGENTS.md "Adding support for ESF-MEAS". UBX_ESF_MEAS_CALLBACK_BUFFERS/UBX_ESF_MEAS_MAX_LEN
// below are still used there (as numCallbackCopies/messageLength), kept as named, easily-tunable
// constants here rather than literals inside the message class - same convention as
// UBX_RXM_SFRBX_CALLBACK_BUFFERS/UBX_RXM_SFRBX_MAX_WORDS. UBX_ESF_MEAS_CALLBACK_BUFFERS has been
// validated against real hardware traffic on a ZED-F9R - 6 and 12 both produced ring-full
// debugPrint warnings (messages being dropped), 18 did not. Same pattern as
// UBX_RXM_SFRBX_CALLBACK_BUFFERS needing to be raised well above its original estimate once
// tested for real - see AGENTS.md "Adding support for ESF-MEAS" for the full detail.
const uint8_t UBX_ESF_MEAS_CALLBACK_BUFFERS = 18;
const uint16_t UBX_ESF_MEAS_MAX_NUM_MEAS = 31; // numMeas is 5 bits, indicating up to 31 groups could be received
const uint16_t UBX_ESF_MEAS_MAX_LEN = 8 + (4 * UBX_ESF_MEAS_MAX_NUM_MEAS) + 4;

// UBX_ESF_MEAS_data_t/UBX_ESF_MEAS_sensorData_t (the wire-format reference structs), the old
// v3-style RAM-management wrapper (ubxESFMEASAutomaticFlags / UBX_ESF_MEAS_t, with its
// callbackPointerPtr/callbackData) have all been removed - ubxESFMEAS is now self-registered
// (ubxMessages/ubxESFMEAS.h) and owns its own storage/callback ring buffer via ubxMessage, and its
// field table is the wire format's documentation now. See AGENTS.md "Adding support for ESF-MEAS"
// and "Tidying up u-blox_structs.h".

// UBX-ESF-RAW (0x10 0x03): Raw sensor measurements
// Note: length is variable
// Note: The ZED-F9R sends sets of seven sensor readings one at a time
//       But the NEO-M8U sends them in sets of ten (i.e. seventy readings per message)
// Note: ESF RAW data cannot be polled. It is "Output" only
// Note: ESF-RAW is now implemented as its own self-registered Class - see ubxMessages/ubxESFRAW.h
// and AGENTS.md "Adding support for ESF-RAW and ESF-STATUS". UBX_ESF_RAW_MAX_LEN below is still
// used there (as messageLength). numCallbackCopies is 1 (a single _callbackStorage buffer,
// explicitly instructed) - the "NEO-M8U sends sets of ten" note above is about how many BLOCKS
// one single message can contain (unchanged, sized for by DEF_NUM_SENS*DEF_MAX_NUM_ESF_RAW_REPEATS
// below), not about needing multiple ring-buffer copies for a burst of separate messages. ESF-RAW
// has no block-count field anywhere in its own wire format (see the numEsfRawBlocks comment in
// ubxMessage.h) - ubxMessage::getBlockCount() falls back to computing the block count purely from
// the actual received message length for a message like this.
const uint16_t UBX_ESF_RAW_MAX_NUM_SENS = 7; // The maximum number of ESF sensors
const uint16_t UBX_ESF_RAW_MAX_NUM_SENS_REPEATS = 1; // The ZED-F9R sends ESF RAW data one block of sensors at a time. The NEO-M8U sends blocks of ten readings.

const uint16_t UBX_ESF_RAW_MAX_LEN = 4 + (8 * UBX_ESF_RAW_MAX_NUM_SENS * UBX_ESF_RAW_MAX_NUM_SENS_REPEATS);

// UBX_ESF_RAW_data_t/UBX_ESF_RAW_sensorData_t (the wire-format reference structs) and the old
// v3-style RAM-management wrapper (UBX_ESF_RAW_t, with its callbackPointerPtr/callbackData) have
// all been removed - ubxESFRAW is now self-registered (ubxMessages/ubxESFRAW.h) and owns its own
// storage/callback buffer via ubxMessage, and its field table is the wire format's documentation
// now. (This wrapper's destructor/autoLookup/processUBXpacket/checkCallbacks cleanup was already
// unreachable dead code before the Phase 31 migration - the pointer that used it,
// packetUBXESFRAW, could never actually be allocated, since the old initPacketUBXESFRAW() was
// declared but never defined.) See AGENTS.md "Adding support for ESF-RAW and ESF-STATUS" and
// "Tidying up u-blox_structs.h".

// UBX-ESF-STATUS (0x10 0x10): External sensor fusion status
// Note: length is variable
// Note: ESF-STATUS is now implemented as its own self-registered Class - see
// ubxMessages/ubxESFSTATUS.h and AGENTS.md "Adding support for ESF-RAW and ESF-STATUS".
// UBX_ESF_STATUS_MAX_LEN below is still used there (as messageLength).
const uint16_t UBX_ESF_STATUS_MAX_NUM_SENS = 7; // The maximum number of ESF sensors
const uint16_t UBX_ESF_STATUS_MAX_LEN = 16 + (4 * UBX_ESF_STATUS_MAX_NUM_SENS);

// UBX_ESF_STATUS_data_t/UBX_ESF_STATUS_sensorStatus_t (the wire-format reference structs) and the
// old v3-style RAM-management wrapper (UBX_ESF_STATUS_moduleQueried_t / UBX_ESF_STATUS_t, with its
// callbackPointerPtr/callbackData) have all been removed - ubxESFSTATUS is now self-registered
// (ubxMessages/ubxESFSTATUS.h) and owns its own storage/callback buffer via ubxMessage, and its
// field table is the wire format's documentation now. See AGENTS.md "Adding support for ESF-RAW
// and ESF-STATUS" and "Tidying up u-blox_structs.h".

// MGA-specific structs

// UBX-MGA-ACK-DATA0 (0x13 0x60): Multiple GNSS acknowledge message
const uint16_t UBX_MGA_ACK_DATA0_LEN = 8;

typedef struct
{
  uint8_t type;               // Type of acknowledgment:
                              // 0: The message was not used by the receiver (see infoCode field for an indication of why)
                              // 1: The message was accepted for use by the receiver (the infoCode field will be 0)
  uint8_t version;            // Message version
  uint8_t infoCode;           // Provides greater information on what the receiver chose to do with the message contents
                              // See sfe_ublox_mga_ack_infocode_e
  uint8_t msgId;              // UBX message ID of the acknowledged message
  uint8_t msgPayloadStart[4]; // The first 4 bytes of the acknowledged message's payload
} UBX_MGA_ACK_DATA0_data_t;

#define UBX_MGA_ACK_DATA0_RINGBUFFER_LEN 16 // Provide storage for 16 MGA ACK packets
typedef struct
{
  uint8_t head;
  uint8_t tail;
  UBX_MGA_ACK_DATA0_data_t data[UBX_MGA_ACK_DATA0_RINGBUFFER_LEN]; // Create a storage array for the MGA ACK packets
} UBX_MGA_ACK_DATA0_t;

// UBX-MGA-DBD (0x13 0x80): Navigation database dump entry
const uint16_t UBX_MGA_DBD_LEN = 164; // "The maximum payload size for firmware 2.01 onwards is 164 bytes"

typedef struct
{
  uint8_t dbdEntryHeader1; // We need to save the entire message - header, payload and checksum
  uint8_t dbdEntryHeader2;
  uint8_t dbdEntryClass;
  uint8_t dbdEntryID;
  uint8_t dbdEntryLenLSB; // We need to store the length of the DBD entry. The entry itself does not contain a length...
  uint8_t dbdEntryLenMSB;
  uint8_t dbdEntry[UBX_MGA_DBD_LEN];
  uint8_t dbdEntryChecksumA;
  uint8_t dbdEntryChecksumB;
} UBX_MGA_DBD_data_t;

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
#define UBX_MGA_DBD_RINGBUFFER_LEN 190 // Fix to let the code compile on AVR platforms - including the UNO and DxCore (DA/DB).
#else
#define UBX_MGA_DBD_RINGBUFFER_LEN 250 // Provide storage for MGA DBD packets. TO DO: confirm if 250 is large enough for all modules!
#endif

typedef struct
{
  uint8_t head;
  uint8_t tail;
  UBX_MGA_DBD_data_t data[UBX_MGA_DBD_RINGBUFFER_LEN]; // Create a storage array for the MGA DBD packets
} UBX_MGA_DBD_t;

// HNR-specific structs

// UBX-HNR-PVT (0x28 0x00): High rate output of PVT solution
const uint16_t UBX_HNR_PVT_LEN = 72;

// UBX-HNR-ATT (0x28 0x01): Attitude solution
const uint16_t UBX_HNR_ATT_LEN = 32;

// UBX-HNR-INS (0x28 0x02): Vehicle dynamics information
const uint16_t UBX_HNR_INS_LEN = 36;

// NMEA-specific structs

// The max length for NMEA messages should be 82 bytes, but GGA messages can exceed that if they include the
// extra decimal places for "High Precision Mode".
//
// To be safe, let's allocate 100 bytes to store the GGA message

const uint8_t NMEA_DTM_MAX_LENGTH = 50;
const uint8_t NMEA_GGA_MAX_LENGTH = 100;
const uint8_t NMEA_GLL_MAX_LENGTH = 50;
const uint8_t NMEA_GNS_MAX_LENGTH = 100;
const uint8_t NMEA_GST_MAX_LENGTH = 100;
const uint8_t NMEA_GSV_MAX_LENGTH = 100; // Variable length - up to 4 satellite blocks. See AGENTS.md "Adding support for NMEA GSV messages"
const uint8_t NMEA_RMC_MAX_LENGTH = 100;
const uint8_t NMEA_VTG_MAX_LENGTH = 50;
const uint8_t NMEA_ZDA_MAX_LENGTH = 50;

typedef struct
{
  uint8_t length;
  uint8_t *data;
} NMEA_STORAGE_t;

// RTCM-specific structs

// Additional flags and pointers that need to be stored with each message type
struct rtcmAutomaticFlags
{
  union
  {
    uint8_t all;
    struct
    {
      uint8_t dataValid : 1;         // Is the copy of the data used by the get function valid/fresh? 0 = invalid, 1 = valid
      uint8_t dataRead : 1;          // Has the data been read? 0 = unread, 1 = read
      uint8_t callbackDataValid : 1; // Is the copy of the data used by the callback valid/fresh? 0 = invalid/stale, 1 = valid/fresh
    } bits;
  } flags;
};

// Maximum length of an RTCM message: 1023
#define SFE_UBLOX_MAX_RTCM_MSG_LEN 1023

typedef struct
{
  uint16_t messageLength;
  uint32_t rollingChecksum;
  uint8_t dataMessage[3 + SFE_UBLOX_MAX_RTCM_MSG_LEN + 3]; // Add extra bytes to hold the preamble, length and CRC
} RTCM_FRAME_t;

// RTCM 1005 Message (0x3ED): Stationary Antenna Reference Point, No Height Information
const uint16_t RTCM_1005_MSG_LEN_BYTES = 19;

typedef struct
{
  uint16_t MessageNumber;                 // Message Number (“1005” = 0x3ED)
  uint16_t ReferenceStationID;            // Reference Station ID
  uint8_t ITRFRealizationYear;            // ITRF Realization Year
  bool GPSIndicator;                      // GPS Indicator
  bool GLONASSIndicator;                  // GLONASS Indicator
  bool GalileoIndicator;                  // Galileo Indicator
  bool ReferenceStationIndicator;         // Reference-Station Indicator
  int64_t AntennaReferencePointECEFX;     // Antenna Reference Point ECEF-X (0.0001m)
  bool SingleReceiverOscillatorIndicator; // Single Receiver Oscillator Indicator
  bool Reserved;                          // Reserved
  int64_t AntennaReferencePointECEFY;     // Antenna Reference Point ECEF-Y (0.0001m)
  uint8_t QuarterCycleIndicator;          // Quarter Cycle Indicator
  int64_t AntennaReferencePointECEFZ;     // Antenna Reference Point ECEF-Z (0.0001m)
} RTCM_1005_data_t;

typedef struct
{
  rtcmAutomaticFlags automaticFlags;
  RTCM_1005_data_t data;
  void (*callbackPointerPtr)(RTCM_1005_data_t *);
  RTCM_1005_data_t *callbackData;
} RTCM_1005_t;

// RTCM 1006 Message (0x3EE): Stationary Antenna Reference Point, with Height Information
// For the u-blox module, RTCM 1006 is input-only. It cannot output RTCM 1006.
const uint16_t RTCM_1006_MSG_LEN_BYTES = 21;

typedef struct
{
  uint16_t MessageNumber;                 // Message Number (“1006” = 0x3EE)
  uint16_t ReferenceStationID;            // Reference Station ID
  uint8_t ITRFRealizationYear;            // ITRF Realization Year
  bool GPSIndicator;                      // GPS Indicator
  bool GLONASSIndicator;                  // GLONASS Indicator
  bool GalileoIndicator;                  // Galileo Indicator
  bool ReferenceStationIndicator;         // Reference-Station Indicator
  int64_t AntennaReferencePointECEFX;     // Antenna Reference Point ECEF-X (0.0001m)
  bool SingleReceiverOscillatorIndicator; // Single Receiver Oscillator Indicator
  bool Reserved;                          // Reserved
  int64_t AntennaReferencePointECEFY;     // Antenna Reference Point ECEF-Y (0.0001m)
  uint8_t QuarterCycleIndicator;          // Quarter Cycle Indicator
  int64_t AntennaReferencePointECEFZ;     // Antenna Reference Point ECEF-Z (0.0001m)
  uint16_t AntennaHeight;                 // Antenna Height above the marker used in the survey campaign (0.0001m)
} RTCM_1006_data_t;
