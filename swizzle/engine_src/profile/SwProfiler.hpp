#ifndef SW_PROFILER_HPP
#define SW_PROFILER_HPP

/* Include files */

#include <swizzle/profile/Profiler.hpp>

#include <fstream>
#include <unordered_map>
#include <array>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::profile
{
    struct Event
    {
        U64 mTimestampUs;
        U32 mThreadId;
        U64 mEventStringId;
        ProfileEventType mEventType;
    };
}

/* Class Declaration */

namespace swizzle::profile
{
    class ProfileContext
    {
    public:
        ProfileContext(const SwChar* filePath);
        ~ProfileContext();

        void registerEvent(const SwChar* evt, ProfileEventType type);

    private:

        std::ofstream mProfileFile;
        U64 mEventCount;

        std::unordered_map<U64, Event> mEvents;

        U64 mStringCount;
        std::unordered_map<const SwChar*, U64> mStringId;

    };
}

/* Function Declaration */


#endif