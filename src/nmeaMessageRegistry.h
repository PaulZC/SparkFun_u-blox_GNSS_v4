#pragma once

#include <vector>

class nmeaMessageRegistry
{
public:
    typedef nmeaMessage *(*nmeaMessageFactoryFn)(void);

    static nmeaMessageRegistry &get(void)
    {
        static nmeaMessageRegistry instance;
        return instance;
    }

    void registerMessage(nmeaMessageFactoryFn factory)
    {
        for (auto existing : _factories)
            if (existing == factory)
                return; // Already registered (e.g. from another translation unit) - skip
        _factories.push_back(factory);
    }

    void buildAll(std::vector<nmeaMessage *> &messages) const
    {
        for (auto factory : _factories)
            messages.push_back(factory());
    }

private:
    nmeaMessageRegistry(void) {}
    std::vector<nmeaMessageFactoryFn> _factories;
};

template <typename MessageType>
class nmeaMessageBuilder
{
public:
    nmeaMessageBuilder(void)
    {
        nmeaMessageRegistry::get().registerMessage(&create);
    }

    static nmeaMessage *create(void)
    {
        return new MessageType();
    }
};

#define nmeaRegisterMessage(kMessage) static nmeaMessageBuilder<kMessage> global_##kMessage##Builder;
