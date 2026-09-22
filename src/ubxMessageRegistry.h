#pragma once

#include <vector>

// v4 scaffolding: message self-registration, modeled on flxDeviceFactory in flux-sdk (see
// AGENTS.md "Message Class self-registration"). Each per-message header (e.g. ubxNAVPVT.h) ends
// with ubxRegisterMessage(ubxNAVPVT), which declares a file-scope static builder object. Its
// constructor runs at static-initialization time (before setup()) purely because the header was
// included somewhere - no central list to edit. ubxMessageVector's constructor then asks this
// registry to build one instance of every registered message.
//
// A header included from more than one translation unit (true here: both u-blox_GNSS.cpp and
// the user's own sketch .ino both pull in every ubxNAVxxx.h transitively via
// SparkFun_u-blox_GNSS_v4.h) gets its own copy of the file-scope static builder per TU, so its
// constructor runs once per TU. Each run calls registerMessage() with the SAME create() function
// pointer though - class-template member functions have "vague linkage" and the linker collapses
// every TU's copy to one address - so registerMessage() de-duplicates by function-pointer value
// to end up with exactly one registration no matter how many TUs included the header.
class ubxMessageRegistry
{
public:
    typedef ubxMessage *(*ubxMessageFactoryFn)(void);

    /**
     * @brief Get the single, lazily-constructed ubxMessageRegistry instance.
     *
     * @return Reference to the process-wide registry singleton.
     */
    static ubxMessageRegistry &get(void)
    {
        static ubxMessageRegistry instance;
        return instance;
    }

    /**
     * @brief Register one message class's factory function, if not already registered.
     *
     * Called by each ubxMessageBuilder<MessageType> at static-initialization time. De-duplicates
     * by function-pointer value, so a header included from multiple translation units still
     * ends up registered exactly once - see the file-level comment above for why.
     *
     * @param factory Function that constructs one instance of the message class being registered.
     */
    void registerMessage(ubxMessageFactoryFn factory)
    {
        for (auto existing : _factories)
            if (existing == factory)
                return; // Already registered (e.g. from another translation unit) - skip
        _factories.push_back(factory);
    }

    /**
     * @brief Construct one instance of every currently-registered message class.
     *
     * @param messages Vector to append each newly-constructed ubxMessage* into (ownership
     * passes to the caller - see ubxMessageVector's constructor/destructor).
     */
    void buildAll(std::vector<ubxMessage *> &messages) const
    {
        for (auto factory : _factories)
            messages.push_back(factory());
    }

private:
    /**
     * @brief Construct the registry. Private - use get() to access the singleton instance.
     */
    ubxMessageRegistry(void) {}
    std::vector<ubxMessageFactoryFn> _factories;
};

template <typename MessageType>
class ubxMessageBuilder
{
public:
    /**
     * @brief Construct a builder and register its MessageType's factory with the registry.
     *
     * Runs at static-initialization time as a side effect of a file-scope
     * ubxMessageBuilder<MessageType> object being declared by the ubxRegisterMessage() macro -
     * see the file-level comment above.
     */
    ubxMessageBuilder(void)
    {
        ubxMessageRegistry::get().registerMessage(&create);
    }

    /**
     * @brief Factory function: construct a new MessageType instance.
     *
     * @return A newly-allocated MessageType, returned as its base ubxMessage* (ownership passes
     * to the caller).
     */
    static ubxMessage *create(void)
    {
        return new MessageType();
    }
};

/**
 * @def ubxRegisterMessage(kMessage)
 * @brief Declare a file-scope static builder that self-registers a message class.
 *
 * Placed at the bottom of each per-message header (e.g. ubxNAVPVT.h). Expands to a
 * file-scope `static ubxMessageBuilder<kMessage> global_##kMessage##Builder;` whose
 * constructor runs at static-initialization time, registering kMessage's factory with
 * ubxMessageRegistry - see the file-level comment above.
 */
#define ubxRegisterMessage(kMessage) static ubxMessageBuilder<kMessage> global_##kMessage##Builder;
