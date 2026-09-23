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
 * @file nmeaMessageRegistry.h
 */

#pragma once

#include <vector>

class nmeaMessageRegistry
{
public:
    typedef nmeaMessage *(*nmeaMessageFactoryFn)(void);

    /**
     * @brief Get the single, lazily-constructed nmeaMessageRegistry instance.
     *
     * @return Reference to the process-wide registry singleton.
     */
    static nmeaMessageRegistry &get(void)
    {
        static nmeaMessageRegistry instance;
        return instance;
    }

    /**
     * @brief Register one NMEA message class's factory function, if not already registered.
     *
     * Called by each nmeaMessageBuilder<MessageType> at static-initialization time.
     * De-duplicates by function-pointer value, mirroring ubxMessageRegistry::registerMessage()
     * - see ubxMessageRegistry.h's file-level comment for why this is needed.
     *
     * @param factory Function that constructs one instance of the message class being registered.
     */
    void registerMessage(nmeaMessageFactoryFn factory)
    {
        for (auto existing : _factories)
            if (existing == factory)
                return; // Already registered (e.g. from another translation unit) - skip
        _factories.push_back(factory);
    }

    /**
     * @brief Construct one instance of every currently-registered NMEA message class.
     *
     * @param messages Vector to append each newly-constructed nmeaMessage* into (ownership
     * passes to the caller - see nmeaMessageVector's constructor/destructor).
     */
    void buildAll(std::vector<nmeaMessage *> &messages) const
    {
        for (auto factory : _factories)
            messages.push_back(factory());
    }

private:
    /**
     * @brief Construct the registry. Private - use get() to access the singleton instance.
     */
    nmeaMessageRegistry(void) {}
    std::vector<nmeaMessageFactoryFn> _factories;
};

template <typename MessageType>
class nmeaMessageBuilder
{
public:
    /**
     * @brief Construct a builder and register its MessageType's factory with the registry.
     *
     * Runs at static-initialization time as a side effect of a file-scope
     * nmeaMessageBuilder<MessageType> object being declared by the nmeaRegisterMessage() macro.
     */
    nmeaMessageBuilder(void)
    {
        nmeaMessageRegistry::get().registerMessage(&create);
    }

    /**
     * @brief Factory function: construct a new MessageType instance.
     *
     * @return A newly-allocated MessageType, returned as its base nmeaMessage* (ownership
     * passes to the caller).
     */
    static nmeaMessage *create(void)
    {
        return new MessageType();
    }
};

/**
 * @def nmeaRegisterMessage(kMessage)
 * @brief Declare a file-scope static builder that self-registers an NMEA message class.
 *
 * Placed at the bottom of each per-message header (e.g. nmeaGGA.h). Mirrors
 * ubxRegisterMessage() - see ubxMessageRegistry.h.
 */
#define nmeaRegisterMessage(kMessage) static nmeaMessageBuilder<kMessage> global_##kMessage##Builder;
