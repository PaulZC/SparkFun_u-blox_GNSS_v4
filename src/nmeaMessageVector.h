/*
  nmeaMessageVector.h

*/

#pragma once

#include <vector>
#include <string.h>

#include "nmeaMessage.h"

// ===========================

// Support for individual NMEA messages is contained in these files
// Individual files can be commented to save both program memory and RAM

#include "nmeaMessages/nmeaGGA.h"
#include "nmeaMessages/nmeaDTM.h"
#include "nmeaMessages/nmeaGLL.h"
#include "nmeaMessages/nmeaGNS.h"
#include "nmeaMessages/nmeaGST.h"
#include "nmeaMessages/nmeaGSV.h"
#include "nmeaMessages/nmeaRMC.h"
#include "nmeaMessages/nmeaVTG.h"
#include "nmeaMessages/nmeaZDA.h"

// ===========================

#include "u-blox_external_typedefs.h" // sfe_ublox_status_e
#include "sfe_debug.h"                 // v4 scaffolding - shared base for debugPrint()/debugPrintln(), see AGENTS.md

class nmeaMessageVector : public SparkFun_UBLOX_GNSS::SfeDebugPrint
{
public:
    std::vector<nmeaMessage *> nmeaMessageVectors;

    nmeaMessageVector(void)
    {
        nmeaMessageRegistry::get().buildAll(nmeaMessageVectors);
    }

    ~nmeaMessageVector(void)
    {
        for (auto msg : nmeaMessageVectors)
            delete msg;
    }

    nmeaMessage *find(const char *msgId)
    {
        for (auto msg : nmeaMessageVectors)
        {
            if (msg->amI(msgId))
                return msg;
        }
        return nullptr;
    }

    // Look up a registered message by its name.
    nmeaMessage *findByName(const char *msgId)
    {
        return find(msgId);
    }

    sfe_ublox_status_e initStorage(const char *msgId)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        return (msg->initStorage() ? SFE_UBLOX_STATUS_SUCCESS : SFE_UBLOX_STATUS_MEM_ERR);
    }

    sfe_ublox_status_e isAutomatic(const char *msgId, bool *automatic)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *automatic = msg->_automatic;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    sfe_ublox_status_e setAutomatic(const char *msgId, bool automatic)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_automatic = automatic;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    sfe_ublox_status_e implicitUpdate(const char *msgId, bool *implicitUpdateOut)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *implicitUpdateOut = msg->_implicitUpdate;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    sfe_ublox_status_e setImplicitUpdate(const char *msgId, bool implicitUpdateIn)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_implicitUpdate = implicitUpdateIn;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    // A single bool per message - has fresh data arrived since the last time it was reported?
    // Not a per-field bitmask - see AGENTS.md "moduleQueried".
    sfe_ublox_status_e moduleQueried(const char *msgId, bool *queried)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *queried = msg->_moduleQueried;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    sfe_ublox_status_e setModuleQueried(const char *msgId, bool queried)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_moduleQueried = queried;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    sfe_ublox_status_e getAddToFileBuffer(const char *msgId, bool *adding)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *adding = msg->_addToFileBuffer;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    sfe_ublox_status_e setAddToFileBuffer(const char *msgId, bool adding)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_addToFileBuffer = adding;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    sfe_ublox_status_e getMsgOutKey(const char *msgId, uint8_t commType, uint32_t *key)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *key = msg->getMsgOutKey(commType);
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    sfe_ublox_status_e setCallback(const char *msgId, void (*callbackPtr)(nmeaCallbackDataCommon_t *))
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_callbackPtr = callbackPtr;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    // Copy 'len' freshly-received payload bytes into this message's storage and mark it fresh.
    // Note: not currently called anywhere - the live NMEA dispatch path is the inline code in
    // DevUBLOXGNSS::process(), which this mirrors (including its ring-buffered callback write
    // side - see AGENTS.md "Adding support for NMEA GSV messages"). Kept working and consistent
    // with the live path rather than removed, in case a future caller wants a single entry point.
    sfe_ublox_status_e storePayload(const char *msgId, const uint8_t *payload, uint16_t len)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        if (!msg->initStorage())
            return SFE_UBLOX_STATUS_MEM_ERR;
        if (len > msg->_messageLength)
            len = msg->_messageLength;
        memcpy(msg->_storage, payload, len);
        msg->_moduleQueried = true;

        if (msg->_callbackPtr != nullptr)
        {
            if (msg->initCallbackStorage())
            {
                if (msg->_numCallbackCopies <= 1)
                {
                    memcpy(msg->_callbackStorage, payload, len);
                    msg->_callbackCount = 1; // head/tail stay at 0 - a degenerate 1-slot ring
                }
                else if (msg->_callbackCount < msg->_numCallbackCopies) // Ring has a free slot
                {
                    uint8_t *slot = msg->_callbackStorage + ((uint32_t)msg->_callbackHead * msg->_messageLength);
                    memcpy(slot, payload, len);
                    msg->_callbackHead = (uint8_t)((msg->_callbackHead + 1) % msg->_numCallbackCopies);
                    msg->_callbackCount++;
                }
                else
                {
                    // ring is full - drop this payload, keep what's already buffered
                    debugPrint("NMEA storePayload: msgId ", true); // Important
                    debugPrint(msgId, true);
                    debugPrintln(" _callbackStorage ring buffer full. Message lost.", true);
                }
            }
        }

        return SFE_UBLOX_STATUS_SUCCESS;
    }

    // Look up one field of one message by name and fill in 'value'.
    sfe_ublox_status_e extractValue(const char *msgId, const char *field, String &value)
    {
        nmeaMessage *msg = find(msgId);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        if (msg->_storage == nullptr)
            return SFE_UBLOX_STATUS_MEM_ERR; // No data has arrived for this message yet

        return msg->extractFieldFrom(msg->_storage, field, value) ? SFE_UBLOX_STATUS_SUCCESS : SFE_UBLOX_STATUS_INVALID_ARG;
    }
};
