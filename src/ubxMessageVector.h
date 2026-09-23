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
 * @file ubxMessageVector.h
 * 
 */

/*
  v4 scaffolding: the registry that owns every compiled-in per-message object and provides
  generic (Class, ID)-keyed dispatch, replacing the per-message switch/if-chains from v3.
  See AGENTS.md "Reference Scaffolding" and "moduleQueried".

  Differences from the preliminary prototype referenced in AGENTS.md (see "Known bugs in the
  prototype" there):
    - Messages are stored as owned pointers (std::vector<ubxMessage *>), not by value - so a
      derived object's _fields table stays valid for the registry's lifetime instead of pointing
      at a destroyed temporary.
    - extractValue() is a complete, working implementation.
    - moduleQueried()/setModuleQueried() operate on a single bool per message (see ubxMessage.h),
      not a per-field bitmask.
*/

#pragma once

#include <vector>
#include <string.h>

#include "ubxMessage.h"

// ===========================

// Support for individual UBX messages is contained in these files
// Individual files can be commented to save both program memory and RAM

#include "ubxMessages/ubxNAVPVT.h"
#include "ubxMessages/ubxNAVPOSECEF.h"
#include "ubxMessages/ubxNAVPOSLLH.h"
#include "ubxMessages/ubxNAVSTATUS.h"
#include "ubxMessages/ubxNAVDOP.h"
#include "ubxMessages/ubxNAVATT.h"
#include "ubxMessages/ubxNAVODO.h"
#include "ubxMessages/ubxNAVVELECEF.h"
#include "ubxMessages/ubxNAVVELNED.h"
#include "ubxMessages/ubxNAVHPPOSECEF.h"
#include "ubxMessages/ubxNAVHPPOSLLH.h"
#include "ubxMessages/ubxNAVPVAT.h"
#include "ubxMessages/ubxNAVTIMEUTC.h"
#include "ubxMessages/ubxNAVCLOCK.h"
#include "ubxMessages/ubxNAVTIMELS.h"
#include "ubxMessages/ubxNAVSVIN.h"
#include "ubxMessages/ubxNAVRELPOSNED.h"
#include "ubxMessages/ubxNAVDAHEADING.h"
#include "ubxMessages/ubxNAVAOPSTATUS.h"
#include "ubxMessages/ubxNAVSAT.h" // Variable-length: header + repeated per-SV blocks - see AGENTS.md "Adding the variable-length UBX messages"
#include "ubxMessages/ubxNAVSIG.h" // Variable-length: header + repeated per-signal blocks - see AGENTS.md "Adding the variable-length UBX messages"
#include "ubxMessages/ubxNAVEOE.h"
#include "ubxMessages/ubxRXMCOR.h"
#include "ubxMessages/ubxRXMMEASX.h" // Variable-length: header + repeated per-satellite blocks - see AGENTS.md "Adding the variable-length UBX messages"
#include "ubxMessages/ubxRXMRAWX.h" // Variable-length: header + repeated per-measurement blocks - see AGENTS.md "Adding the variable-length UBX messages"
#include "ubxMessages/ubxRXMSFRBX.h" // Variable-length: header + repeated 4-byte data words, ring-buffered callback storage (numCallbackCopies=14) - see AGENTS.md "Adding support for RXM-SFRBX"
#include "ubxMessages/ubxRXMPMP.h" // Variable-length: header + repeated 1-byte userData "blocks" - only Version 0x01's layout is modelled, see ubxRXMPMP.h
#include "ubxMessages/ubxRXMQZSSL6.h" // Fixed-length: header + repeated 1-byte msgBytes "blocks", numCallbackCopies=UBX_RXM_QZSSL6_NUM_CHANNELS (2) - see ubxRXMQZSSL6.h
#include "ubxMessages/ubxMONCOMMS.h"
#include "ubxMessages/ubxMONRF.h" // Variable-length: header + repeated per-RF-block blocks, replaces the old getRFinformation() - see AGENTS.md "Adding the variable-length UBX messages"
#include "ubxMessages/ubxMONHW.h"
#include "ubxMessages/ubxMONHW2.h"
#include "ubxMessages/ubxTIMTM2.h"
#include "ubxMessages/ubxTIMTP.h"
#include "ubxMessages/ubxESFALG.h"
#include "ubxMessages/ubxESFINS.h"
#include "ubxMessages/ubxHNRPVT.h"
#include "ubxMessages/ubxHNRATT.h"
#include "ubxMessages/ubxHNRINS.h"
#include "ubxMessages/ubxSECSIG.h" // Variable-length (Version 3 - see ubxSECSIG.h): header + repeated per-frequency blocks - see AGENTS.md "Adding the variable-length UBX messages"
#include "ubxMessages/ubxSECUNIQID.h" // Fixed-shape: header + uniqueId modelled as repeated 1-byte "blocks" (5 or 6, version-dependent, derived from actual length like ESF-RAW), replaces the old getUniqueChipId()/getUniqueChipIdStr() - see ubxSECUNIQID.h
#include "ubxMessages/ubxESFMEAS.h" // Variable-length: header + repeated per-measurement blocks + an optional footer, ring-buffered callback storage (numCallbackCopies=UBX_ESF_MEAS_CALLBACK_BUFFERS) - see AGENTS.md "Adding support for ESF-MEAS"
#include "ubxMessages/ubxESFRAW.h" // Variable-length: NO header count field at all, block count derived purely from actual received length - see AGENTS.md "Adding support for ESF-RAW and ESF-STATUS"
#include "ubxMessages/ubxESFSTATUS.h" // Variable-length: header + repeated per-sensor status blocks - see AGENTS.md "Adding support for ESF-RAW and ESF-STATUS"

// ===========================

#include "u-blox_external_typedefs.h" // sfe_ublox_status_e
#include "u-blox_config_keys.h"       // UBX_CFG_* type tags
#include "sfe_debug.h"                // v4 scaffolding - shared base for debugPrint()/debugPrintln(), see AGENTS.md

class ubxMessageVector : public SparkFun_UBLOX_GNSS::SfeDebugPrint
{
public:
    std::vector<ubxMessage *> ubxMessageVectors;

    /**
     * @brief Construct the vector and populate it with one instance of every registered message.
     *
     * Asks ubxMessageRegistry to build one instance of every message class that self-registered
     * via ubxRegisterMessage() in its own header - see the class-level comment above and
     * AGENTS.md "Message Class self-registration".
     */
    ubxMessageVector(void)
    {
        // Each header included above (e.g. ubxNAVPVT.h) self-registers a builder for its
        // message class via ubxRegisterMessage() when the header is compiled in - see
        // AGENTS.md "Message Class self-registration". This asks the registry to instantiate
        // one instance of every message class that self-registered. To drop support for a
        // message, comment out its #include above - nothing here needs to change.
        ubxMessageRegistry::get().buildAll(ubxMessageVectors);
    }

    /**
     * @brief Destroy the vector, deleting every message object it owns.
     */
    ~ubxMessageVector(void)
    {
        for (auto msg : ubxMessageVectors)
            delete msg;
    }

    /**
     * @brief Find the registered message object for a given UBX Class/ID.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @return Pointer to the matching ubxMessage, or nullptr if no message is registered for
     * that Class/ID.
     */
    ubxMessage *find(uint8_t Class, uint8_t ID)
    {
        for (auto msg : ubxMessageVectors)
        {
            if (msg->amI(Class, ID))
                return msg;
        }
        return nullptr;
    }

    // Look up a registered message by its classStr/idStr names (e.g. "NAV", "HPPOSLLH") rather
    // than its numeric Class/ID - backs DevUBLOXGNSS::setAutoCallbackPtr(), which takes names
    // because that's what a user calling it from a sketch has to hand, per AGENTS.md
    // "setAutoCallbackPtr" and CallbackExample1_NAVHPPOSLLH.ino.
    /**
     * @brief Find the registered message object by its human-readable class/id name.
     *
     * @param classStr Class mnemonic to match (e.g. "NAV").
     * @param idStr Message mnemonic to match (e.g. "HPPOSLLH").
     * @return Pointer to the matching ubxMessage, or nullptr if no message is registered under
     * that name pair.
     */
    ubxMessage *findByName(const char *classStr, const char *idStr)
    {
        for (auto msg : ubxMessageVectors)
        {
            if ((strcmp(msg->_classStr, classStr) == 0) && (strcmp(msg->_idStr, idStr) == 0))
                return msg;
        }
        return nullptr;
    }

    /**
     * @brief Lazily allocate a registered message's raw payload storage.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID, or SFE_UBLOX_STATUS_MEM_ERR if the allocation failed.
     */
    sfe_ublox_status_e initStorage(uint8_t Class, uint8_t ID)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        return (msg->initStorage() ? SFE_UBLOX_STATUS_SUCCESS : SFE_UBLOX_STATUS_MEM_ERR);
    }

    /**
     * @brief Get whether a registered message is set to be output periodically by the module.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param automatic Out parameter: filled in with the message's automatic-output flag.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e isAutomatic(uint8_t Class, uint8_t ID, bool *automatic)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *automatic = msg->_automatic;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    /**
     * @brief Set whether a registered message is set to be output periodically by the module.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param automatic The new automatic-output flag value.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e setAutomatic(uint8_t Class, uint8_t ID, bool automatic)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_automatic = automatic;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    /**
     * @brief Get a registered message's implicit-update flag.
     *
     * true means the generic getUBX()-style call itself parses newly-arrived data for this
     * message; false means the caller must call checkUblox() itself first.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param implicitUpdateOut Out parameter: filled in with the message's implicit-update flag.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e implicitUpdate(uint8_t Class, uint8_t ID, bool *implicitUpdateOut)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *implicitUpdateOut = msg->_implicitUpdate;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    /**
     * @brief Set a registered message's implicit-update flag.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param implicitUpdateIn The new implicit-update flag value.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e setImplicitUpdate(uint8_t Class, uint8_t ID, bool implicitUpdateIn)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_implicitUpdate = implicitUpdateIn;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    // A single bool per message - has fresh data arrived since the last time it was reported?
    // Not a per-field bitmask - see AGENTS.md "moduleQueried".
    /**
     * @brief Get whether fresh data has arrived for a registered message since it was last reported.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param queried Out parameter: filled in with the message's moduleQueried flag.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e moduleQueried(uint8_t Class, uint8_t ID, bool *queried)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *queried = msg->_moduleQueried;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    /**
     * @brief Set whether fresh data has arrived for a registered message since it was last reported.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param queried The new moduleQueried flag value.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e setModuleQueried(uint8_t Class, uint8_t ID, bool queried)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_moduleQueried = queried;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    /**
     * @brief Get whether a registered message is set to be added to the file buffer (logUBX()).
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param adding Out parameter: filled in with the message's addToFileBuffer flag.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e getAddToFileBuffer(uint8_t Class, uint8_t ID, bool *adding)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *adding = msg->_addToFileBuffer;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    /**
     * @brief Set whether a registered message is added to the file buffer (used by logUBX()).
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param adding The new addToFileBuffer flag value.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e setAddToFileBuffer(uint8_t Class, uint8_t ID, bool adding)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_addToFileBuffer = adding;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    /**
     * @brief Get a registered message's CFG-MSGOUT key for a given communication port.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param commType Which port's key to return (I2C, SPI, UART1, UART2).
     * @param key Out parameter: filled in with the UBLOX_CFG_MSGOUT_* key.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e getMsgOutKey(uint8_t Class, uint8_t ID, uint8_t commType, uint32_t *key)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *key = msg->getMsgOutKey(commType);
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    /**
     * @brief Register (or clear) the user callback function for a registered message.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param callbackPtr Function to call once fresh data is waiting for this message, or
     * nullptr to clear any previously-registered callback.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID.
     */
    sfe_ublox_status_e setCallback(uint8_t Class, uint8_t ID, void (*callbackPtr)(ubxCallbackDataCommon_t *))
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_callbackPtr = callbackPtr;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    // Copy 'len' freshly-received payload bytes into this message's storage and mark it fresh.
    // This is the receive-side hook that AGENTS.md flags as "the largest remaining piece of
    // design work" for v3 messages in general; it is wired up for the messages migrated so far -
    // see DevUBLOXGNSS::processUBXpacket() in u-blox_GNSS.cpp.
    // 'checksumA'/'checksumB' are the already-validated checksum bytes from the incoming ubxPacket
    // (its one call site, processUBXpacket(), has them straight from msg->checksumA/checksumB) -
    // needed to synthesize the complete raw UBX frame for _callbackRawFrame - see AGENTS.md
    // "Adding support for ESF-MEAS" and ubxMessage::writeCallbackRawFrame().
    /**
     * @brief Copy freshly-received payload bytes into a registered message's storage.
     *
     * Copies into the message's live _storage (marking it fresh via _moduleQueried), and, if a
     * callback is registered, also freezes a copy into the message's separate ring-buffered
     * _callbackStorage - see the longer design note above this function's definition for how
     * the ring buffer is filled and what happens when it is full.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param payload Pointer to the newly-received payload bytes.
     * @param len Number of payload bytes received (clamped to the message's _messageLength
     * before being copied/stored).
     * @param checksumA First UBX checksum byte (CK_A) of the received frame, needed to
     * synthesize a complete raw frame for the callback path.
     * @param checksumB Second UBX checksum byte (CK_B) of the received frame.
     * @return SFE_UBLOX_STATUS_SUCCESS on success, SFE_UBLOX_STATUS_INVALID_ARG if no message
     * is registered for Class/ID, or SFE_UBLOX_STATUS_MEM_ERR if storage could not be allocated.
     */
    sfe_ublox_status_e storePayload(uint8_t Class, uint8_t ID, const uint8_t *payload, uint16_t len, uint8_t checksumA, uint8_t checksumB)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        if (!msg->initStorage())
            return SFE_UBLOX_STATUS_MEM_ERR;
        if (len > msg->_messageLength)
            len = msg->_messageLength;
        memcpy(msg->_storage, payload, len);
        // The real received payload length actually copied above - NOT necessarily _messageLength
        // (that's only the allocated maximum). See AGENTS.md "Adding support for ESF-MEAS": bytes
        // beyond 'len' in _storage can be stale leftovers from a previous, longer message, since
        // this memcpy (and initStorage()'s one-time zero-fill) never clears them - a defensive
        // reader (see ubxMessage::getBlockCount()/extractFooterFieldFrom()) needs this actual
        // length, not just the message's own header count field, to know where real data ends.
        msg->_actualLength = len;
        msg->_moduleQueried = true;

        // v4 scaffolding: if a callback has been registered (see DevUBLOXGNSS::setAutoCallbackPtr()
        // / AGENTS.md "setAutoCallbackPtr"), also freeze this payload into the message's separate
        // _callbackStorage - a ring buffer of _numCallbackCopies slots - so DevUBLOXGNSS::checkCallbacks()
        // has stable copies to hand the user's callback even if _storage gets overwritten by the next
        // message before checkCallbacks() next runs, and so a burst of several messages arriving in one
        // checkUblox() call (e.g. RXM-SFRBX) isn't collapsed down to just the latest one - see AGENTS.md
        // "class ubxMessage needs separate callback storage" and "Adding support for RXM-SFRBX".
        if ((msg->_callbackPtr != nullptr) && msg->initCallbackStorage())
        {
            if (msg->_numCallbackCopies <= 1)
            {
                // Unchanged from the original single-slot behavior: always overwrite the one slot
                // with the newest data (latest wins) - the right behavior for a "give me the latest"
                // message like NAV-PVT. This is every message registered so far except RXM-SFRBX
                // and ESF-MEAS.
                memcpy(msg->_callbackStorage, payload, len);
                msg->_callbackActualLength[0] = len;
                msg->writeCallbackRawFrame(0, payload, len, checksumA, checksumB);
                msg->_callbackCount = 1; // head/tail stay at 0 - a degenerate 1-slot ring
            }
            else if (msg->_callbackCount < msg->_numCallbackCopies) // Ring has a free slot
            {
                uint8_t *slot = msg->_callbackStorage + ((uint32_t)msg->_callbackHead * msg->_messageLength);
                memcpy(slot, payload, len);
                msg->_callbackActualLength[msg->_callbackHead] = len;
                msg->writeCallbackRawFrame(msg->_callbackHead, payload, len, checksumA, checksumB);
                msg->_callbackHead = (uint8_t)((msg->_callbackHead + 1) % msg->_numCallbackCopies);
                msg->_callbackCount++;
            }
            else
            {
                // Ring is full - drop this one and keep what's already buffered, rather than
                // overwriting unread data (per AGENTS.md "Adding support for RXM-SFRBX": write to
                // _head "if space is available"). Every individual SFRBX/ESF-MEAS message matters to
                // a downstream decoder, so silently replacing a buffered-but-unread one would be the
                // wrong failure mode here - this only happens if checkCallbacks() falls behind.
                debugPrint("UBX storePayload: Class 0x", true); // Important
                debugPrint(Class, HEX, true);
                debugPrint(" ID 0x", true);
                debugPrint(ID, HEX, true);
                debugPrintln(" _callbackStorage ring buffer full. Message lost.", true);
            }
        }

        return SFE_UBLOX_STATUS_SUCCESS;
    }

    // Look up one field of one message by name and fill in 'value'. Backs
    // DevUBLOXGNSS::getUBXfield() in u-blox_GNSS.cpp. The actual field-table walk and byte/bit
    // extraction now live once, on ubxMessage itself (extractFieldFrom()), so the same code also
    // backs the callback read path (getUbxMessageField()) - see ubxMessage.h.
    /**
     * @brief Look up one named field of a registered message's live payload.
     *
     * @param Class The UBX message class to look up.
     * @param ID The UBX message ID (within Class) to look up.
     * @param field Name of the field to extract.
     * @param value Out parameter: filled in with the field's tagged value if found.
     * @return SFE_UBLOX_STATUS_SUCCESS if found, SFE_UBLOX_STATUS_INVALID_ARG if no message is
     * registered for Class/ID or the field was not found, or SFE_UBLOX_STATUS_MEM_ERR if no
     * data has arrived for this message yet (its _storage is still unallocated).
     */
    sfe_ublox_status_e extractValue(uint8_t Class, uint8_t ID, const char *field, ubxAnyType *value)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        if (msg->_storage == nullptr)
            return SFE_UBLOX_STATUS_MEM_ERR; // No data has arrived for this message yet

        return msg->extractFieldFrom(msg->_storage, field, value) ? SFE_UBLOX_STATUS_SUCCESS : SFE_UBLOX_STATUS_INVALID_ARG;
    }
};
