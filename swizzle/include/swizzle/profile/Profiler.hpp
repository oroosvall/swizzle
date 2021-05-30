#ifndef PROFILER_HPP
#define PROFILER_HPP

/* Include files */

#include <swizzle/Api.hpp>
#include <common/Common.hpp>
#include <string>

/* Defines */

#define PROFILE_EVENT(evt, type) swizzle::profile::ProfileEvent(evt, type)

#if defined(OTP_PROFILING)
#define PROFILE_EVENT_DBG(evt, type) swizzle::profile::ProfileEvent(evt, type)

#else
#define PROFILE_EVENT_DBG(evt, type)

#endif

/* Typedefs/enums */

namespace swizzle::profile
{
    enum class ProfileEventType
    {
        EventType_Generic = 0x0,
        EventType_ProfileStart = 0x1,
        EventType_ProfileEnd = 0x2,
        EventType_Frame = 0x3,
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::profile
{
    class ProfileScope
    {
    public:
        inline ProfileScope(const SwChar* scopeName)
            : mScopeName(scopeName)
        {
            std::string str = mScopeName;
            str += "-start";
            //PROFILE_EVENT_DBG(str.c_str());
        }

        inline ~ProfileScope()
        {
            std::string str = mScopeName;
            str += "-end";
            //PROFILE_EVENT_DBG(str.c_str());
        }

    private:
        const SwChar* mScopeName;
    };
}

/* Function Declaration */

namespace swizzle::profile
{
    void SWIZZLE_API ProfileStart(const SwChar* profilerFile);
    void SWIZZLE_API ProfileEnd();

    void SWIZZLE_API ProfileEvent(const SwChar* evt, ProfileEventType type);


    void SWIZZLE_API ProfileThreadStart(const SwChar* thrdName);
    void SWIZZLE_API ProfileThreadEnd();
}

#endif