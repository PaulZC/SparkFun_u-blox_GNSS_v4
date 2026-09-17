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

    static ubxMessageRegistry &get(void)
    {
        static ubxMessageRegistry instance;
        return instance;
    }

    void registerMessage(ubxMessageFactoryFn factory)
    {
        for (auto existing : _factories)
            if (existing == factory)
                return; // Already registered (e.g. from another translation unit) - skip
        _factories.push_back(factory);
    }

    void buildAll(std::vector<ubxMessage *> &messages) const
    {
        for (auto factory : _factories)
            messages.push_back(factory());
    }

private:
    ubxMessageRegistry(void) {}
    std::vector<ubxMessageFactoryFn> _factories;
};

template <typename MessageType>
class ubxMessageBuilder
{
public:
    ubxMessageBuilder(void)
    {
        ubxMessageRegistry::get().registerMessage(&create);
    }

    static ubxMessage *create(void)
    {
        return new MessageType();
    }
};

#define ubxRegisterMessage(kMessage) static ubxMessageBuilder<kMessage> global_##kMessage##Builder;
