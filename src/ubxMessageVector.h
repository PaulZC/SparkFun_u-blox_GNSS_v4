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

    sfe_ublox_status_e getMsgOutKey(uint8_t Class, uint8_t ID, uint8_t commType, uint32_t *key)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        *key = msg->getMsgOutKey(commType);
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    sfe_ublox_status_e setCallback(uint8_t Class, uint8_t ID, void (*callbackPtr)(uint8_t *))
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        msg->_callbackPtr = callbackPtr;
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    // Copy 'len' freshly-received payload bytes into this message's storage and mark it fresh.
    // This is the receive-side hook that AGENTS.md flags as "the largest remaining piece of
    // design work" for v3 messages in general; it is wired up for NAV-PVT only so far - see
    // DevUBLOXGNSS::processUBXpacket() in u-blox_GNSS.cpp.
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
        return SFE_UBLOX_STATUS_SUCCESS;
    }

    // Extract 'width' (1, 2, 4 or 8) little-endian bytes starting at byte offset 'offset' as an
    // unsigned value.
    static uint64_t extractUnsignedBytes(const uint8_t *storage, uint16_t offset, uint8_t width)
    {
        uint64_t val = 0;
        for (uint8_t i = 0; i < width; i++)
            val |= ((uint64_t)storage[offset + i]) << (8 * i);
        return val;
    }

    // Extract 'bitWidth' bits (<= 32) starting at bit 'startBit' within the byte at 'offset'.
    static uint32_t extractBits(const uint8_t *storage, uint16_t offset, uint8_t startBit, uint8_t bitWidth)
    {
        uint32_t acc = 0;
        uint8_t bytesNeeded = (uint8_t)((startBit + bitWidth + 7) / 8);
        for (uint8_t i = 0; i < bytesNeeded; i++)
            acc |= ((uint32_t)storage[offset + i]) << (8 * i);
        uint32_t mask = (bitWidth >= 32) ? 0xFFFFFFFFu : (uint32_t)((1UL << bitWidth) - 1UL);
        return (acc >> startBit) & mask;
    }

    // Look up one field of one message by name and fill in 'value'. Backs
    // DevUBLOXGNSS::getUBXfield() in u-blox_GNSS.cpp.
    sfe_ublox_status_e extractValue(uint8_t Class, uint8_t ID, const char *field, ubxAnyType *value)
    {
        ubxMessage *msg = find(Class, ID);
        if (msg == nullptr)
            return SFE_UBLOX_STATUS_INVALID_ARG;
        if (msg->_storage == nullptr)
            return SFE_UBLOX_STATUS_MEM_ERR; // No data has arrived for this message yet

        const ubxMessage::ubxField *fields = (const ubxMessage::ubxField *)msg->_fields;
        for (uint8_t i = 0; i < msg->_numFields; i++)
        {
            if (strncmp(fields[i].fieldName, field, sizeof(fields[i].fieldName)) != 0)
                continue;

            value->ubxDataType = fields[i].ubxDataType;

            if (fields[i].startBit >= 0) // A sub-field: always extracted as an unsigned value
            {
                value->U4 = extractBits(msg->_storage, fields[i].startByte, (uint8_t)fields[i].startBit, (uint8_t)fields[i].bitWidth);
                return SFE_UBLOX_STATUS_SUCCESS;
            }

            switch (fields[i].ubxDataType)
            {
            case ubxDataType8bit(UBX_CFG_L):
                value->L = (bool)msg->_storage[fields[i].startByte];
                return SFE_UBLOX_STATUS_SUCCESS;
            case ubxDataType8bit(UBX_CFG_U1):
            case ubxDataType8bit(UBX_CFG_E1):
            case ubxDataType8bit(UBX_CFG_X1):
                value->U1 = (uint8_t)extractUnsignedBytes(msg->_storage, fields[i].startByte, 1);
                return SFE_UBLOX_STATUS_SUCCESS;
            case ubxDataType8bit(UBX_CFG_I1):
                value->I1 = (int8_t)extractUnsignedBytes(msg->_storage, fields[i].startByte, 1);
                return SFE_UBLOX_STATUS_SUCCESS;
            case ubxDataType8bit(UBX_CFG_U2):
            case ubxDataType8bit(UBX_CFG_E2):
            case ubxDataType8bit(UBX_CFG_X2):
                value->U2 = (uint16_t)extractUnsignedBytes(msg->_storage, fields[i].startByte, 2);
                return SFE_UBLOX_STATUS_SUCCESS;
            case ubxDataType8bit(UBX_CFG_I2):
                value->I2 = (int16_t)extractUnsignedBytes(msg->_storage, fields[i].startByte, 2);
                return SFE_UBLOX_STATUS_SUCCESS;
            case ubxDataType8bit(UBX_CFG_U4):
            case ubxDataType8bit(UBX_CFG_E4):
            case ubxDataType8bit(UBX_CFG_X4):
                value->U4 = (uint32_t)extractUnsignedBytes(msg->_storage, fields[i].startByte, 4);
                return SFE_UBLOX_STATUS_SUCCESS;
            case ubxDataType8bit(UBX_CFG_I4):
                value->I4 = (int32_t)extractUnsignedBytes(msg->_storage, fields[i].startByte, 4);
                return SFE_UBLOX_STATUS_SUCCESS;
            case ubxDataType8bit(UBX_CFG_R4):
            {
                uint32_t bits = (uint32_t)extractUnsignedBytes(msg->_storage, fields[i].startByte, 4);
                memcpy(&value->R4, &bits, sizeof(float));
                return SFE_UBLOX_STATUS_SUCCESS;
            }
            case ubxDataType8bit(UBX_CFG_U8):
            case ubxDataType8bit(UBX_CFG_X8):
                value->U8 = extractUnsignedBytes(msg->_storage, fields[i].startByte, 8);
                return SFE_UBLOX_STATUS_SUCCESS;
            case ubxDataType8bit(UBX_CFG_I8):
                value->I8 = (int64_t)extractUnsignedBytes(msg->_storage, fields[i].startByte, 8);
                return SFE_UBLOX_STATUS_SUCCESS;
            case ubxDataType8bit(UBX_CFG_R8):
            {
                uint64_t bits = extractUnsignedBytes(msg->_storage, fields[i].startByte, 8);
                memcpy(&value->R8, &bits, sizeof(double));
                return SFE_UBLOX_STATUS_SUCCESS;
            }
            default:
                return SFE_UBLOX_STATUS_FAIL; // Unknown ubxDataType
            }
        }
        return SFE_UBLOX_STATUS_INVALID_ARG; // Field name not found
    }
};
