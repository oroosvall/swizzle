#ifndef EVENT_HPP
#define EVENT_HPP

#include <swizzle/Api.hpp>

namespace swizzle
{

    template <typename T> class EventHandler
    {
    public:
        virtual void publishEvent(const T&) = 0;
    };

} // namespace swizzle

#endif