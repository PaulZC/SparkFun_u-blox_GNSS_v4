/*
  ubxMessageVector.h

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
#include "ubxNAVPVT.h"
#include "ubxNAVPOSECEF.h"
#include "ubxNAVPOSLLH.h"
#include "ubxNAVSTATUS.h"
#include "ubxNAVDOP.h"
#include "ubxNAVATT.h"
#include "ubxNAVODO.h"
#include "ubxNAVVELECEF.h"
#include "ubxNAVVELNED.h"
#include "ubxNAVHPPOSECEF.h"
#include "ubxNAVHPPOSLLH.h"
#include "ubxNAVPVAT.h"
#include "ubxNAVTIMEUTC.h"
#include "ubxNAVCLOCK.h"
#include "ubxNAVTIMELS.h"
#include "ubxNAVSVIN.h"
#include "ubxNAVRELPOSNED.h"
#include "ubxNAVDAHEADING.h"
#include "ubxNAVAOPSTATUS.h"
#include "ubxNAVEOE.h"
#include "ubxRXMCOR.h"
#include "ubxMONHW.h"
#include "ubxMONHW2.h"
#include "ubxTIMTM2.h"
#include "ubxTIMTP.h"
#include "ubxESFALG.h"
#include "ubxESFINS.h"
#include "ubxHNRPVT.h"
#include "ubxHNRATT.h"
#include "ubxHNRINS.h"
#include "u-blox_external_typedefs.h" // sfe_ublox_status_e
#include "u-blox_config_keys.h"       // UBX_CFG_* type tags

class ubxMessageVector
{
public:
    std::vector<ubxMessage *> ubxMessageVectors;

    ubxMessageVector(void)
    {
        // Each supported message is instantiated exactly once, here. This is the
        // "self-registration" step described in AGENTS.md - see "Still undesigned even after
        // this prototype" there for the conditional-registration mechanism this still needs
        // (today, deleting a message header also requires removing its constructor call here).
        ubxMessageVectors.push_back(new ubxNAVPVT());
        ubxMessageVectors.push_back(new ubxNAVPOSECEF());
        ubxMessageVectors.push_back(new ubxNAVPOSLLH());
        ubxMessageVectors.push_back(new ubxNAVSTATUS());
        ubxMessageVectors.push_back(new ubxNAVDOP());
        ubxMessageVectors.push_back(new ubxNAVATT());
        ubxMessageVectors.push_back(new ubxNAVODO());
        ubxMessageVectors.push_back(new ubxNAVVELECEF());
        ubxMessageVectors.push_back(new ubxNAVVELNED());
        ubxMessageVectors.push_back(new ubxNAVHPPOSECEF());
        ubxMessageVectors.push_back(new ubxNAVHPPOSLLH());
        ubxMessageVectors.push_back(new ubxNAVPVAT());
        ubxMessageVectors.push_back(new ubxNAVTIMEUTC());
        ubxMessageVectors.push_back(new ubxNAVCLOCK());
        ubxMessageVectors.push_back(new ubxNAVTIMELS());
        ubxMessageVectors.push_back(new ubxNAVSVIN());
        ubxMessageVectors.push_back(new ubxNAVRELPOSNED());
        ubxMessageVectors.push_back(new ubxNAVDAHEADING());
        ubxMessageVectors.push_back(new ubxNAVAOPSTATUS());
        ubxMessageVectors.push_back(new ubxNAVEOE());
        ubxMessageVectors.push_back(new ubxRXMCOR());
        ubxMessageVectors.push_back(new ubxMONHW());
        ubxMessageVectors.push_back(new ubxMONHW2());
        ubxMessageVectors.push_back(new ubxTIMTM2());
        ubxMessageVectors.push_back(new ubxTIMTP());
        ubxMessageVectors.push_back(new ubxESFALG());
        ubxMessageVectors.push_back(new ubxESFINS());
        ubxMessageVectors.push_back(new ubxHNRPVT());
        ubxMessageVectors.push_back(new ubxHNRATT());
        ubxMessageVectors.push_back(new ubxHNRINS());
    }

    ~ubxMessageVector(void)
    {
        for (auto msg : ubxMessageVectors)
            delete msg;
    }

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
    ubxMessage *findByName(const char *classStr, const char *idStr)
    {
        for (auto msg : ubxMessageVectors)
        {
            if ((strcmp(msg->_classStr, classStr) == 0) && (strcmp(msg->_idStr, idStr) == 0))
                return msg;
        }
        return nullptr;
    }

    sfe_ublox_status_e initStorage(uint8_t Class, uint8_t ID)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        return (msg->initStorage() ? SFE_UBLOX_STATUS_SUCCESS : SFE_UBLOX_STATUS_MEM_ERR);
    }

    sfe_ublox_status_e isAutomatic(uint8_t Class, uint8_t ID, bool *automatic)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *automatic = msg->_automatic;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    sfe_ublox_status_e setAutomatic(uint8_t Class, uint8_t ID, bool automatic)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_automatic = automatic;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    sfe_ublox_status_e implicitUpdate(uint8_t Class, uint8_t ID, bool *implicitUpdateOut)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *implicitUpdateOut = msg->_implicitUpdate;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
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
    sfe_ublox_status_e moduleQueried(uint8_t Class, uint8_t ID, bool *queried)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *queried = msg->_moduleQueried;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    sfe_ublox_status_e setModuleQueried(uint8_t Class, uint8_t ID, bool queried)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_moduleQueried = queried;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    sfe_ublox_status_e getAddToFileBuffer(uint8_t Class, uint8_t ID, bool *adding)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *adding = msg->_addToFileBuffer;
        return SFE_UBLOX_STATUS_SUCCESS;
    }
    sfe_ublox_status_e setAddToFileBuffer(uint8_t Class, uint8_t ID, bool adding)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_addToFileBuffer = adding;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    sfe_ublox_status_e getMsgOutKey(uint8_t Class, uint8_t ID, uint8_t commType, uint32_t *key)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *key = msg->getMsgOutKey(commType);
        return SFE_UBLOX_STATUS_SUCCESS;
    }

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
    sfe_ublox_status_e storePayload(uint8_t Class, uint8_t ID, const uint8_t *payload, uint16_t len)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        if (!msg->initStorage())
            return SFE_UBLOX_STATUS_MEM_ERR;
        if (len > msg->_messageLength)
            len = msg->_messageLength;
        memcpy(msg->_storage, payload, len);
        msg->_moduleQueried = true;

        // v4 scaffolding: if a callback has been registered (see DevUBLOXGNSS::setAutoCallbackPtr()
        // / AGENTS.md "setAutoCallbackPtr"), also freeze this payload into the message's separate
        // _callbackStorage and mark it pending, so DevUBLOXGNSS::checkCallbacks() has a stable copy
        // to hand the user's callback even if _storage gets overwritten by the next message before
        // checkCallbacks() next runs - see AGENTS.md "class ubxMessage needs separate callback
        // storage" and "Future work". Only a single callback copy is kept (matches
        // _numCallbackCopies == 1 for every currently-registered message); ring-buffered multi-copy
        // support for RXM-SFRBX/ESF-MEAS is still future work, per AGENTS.md.
        if (msg->_callbackPtr != nullptr)
        {
            if (msg->initCallbackStorage())
            {
                memcpy(msg->_callbackStorage, payload, len);
                msg->_callbackDataValid = true;
            }
        }

        return SFE_UBLOX_STATUS_SUCCESS;
    }

    // Look up one field of one message by name and fill in 'value'. Backs
    // DevUBLOXGNSS::getUBXfield() in u-blox_GNSS.cpp. The actual field-table walk and byte/bit
    // extraction now live once, on ubxMessage itself (extractFieldFrom()), so the same code also
    // backs the callback read path (getUbxMessageField()) - see ubxMessage.h.
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
