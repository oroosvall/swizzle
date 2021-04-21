
/* Include files */

#include "SwProfiler.hpp"

#include "core/PlatformLayer.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

swizzle::profile::ProfileContext* gProfileContext = nullptr;

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::profile
{
    void ProfileStart(const SwChar* profilerFile)
    {
        gProfileContext = new ProfileContext(profilerFile);
    }

    void ProfileEnd()
    {
        delete gProfileContext;
        gProfileContext = nullptr;
    }

    void ProfileEvent(const SwChar* evt, ProfileEventType type)
    {
        if (gProfileContext)
        {
            gProfileContext->registerEvent(evt, type);
        }
    }

    void ProfileThreadStart(const SwChar* thrdName)
    {
        (void)thrdName;
    }

    void ProfileThreadEnd()
    {

    }
}

/* Class Public Function Definition */

namespace swizzle::profile
{

    ProfileContext::ProfileContext(const SwChar* filePath)
        : mProfileFile()
        , mEventCount(0u)
        , mEvents()
        , mStringCount(0u)
        , mStringId()
    {
        mProfileFile.open(filePath, std::ios::binary | std::ios::out);
        registerEvent("Profile Start", ProfileEventType::EventType_ProfileStart);
    }

    ProfileContext::~ProfileContext()
    {
        registerEvent("Profile End", ProfileEventType::EventType_ProfileStart);

        U64 stringCount = (U64)mStringId.size();
        mProfileFile.write((char*)&stringCount, sizeof(stringCount));

        for (auto& it: mStringId)
        {
            const SwChar* str = it.first;
            U64 id = it.second;
            mProfileFile.write((char*)&id, sizeof(id));

            std::string string(str);
            U64 len = string.size();

            mProfileFile.write((char*)&len, sizeof(len));
            mProfileFile.write(str, len);
        }


        U64 eventCount = (U64)mEvents.size();
        mProfileFile.write((char*)&eventCount, sizeof(eventCount));

        for (U64 i = 0u; i < eventCount; ++i)
        {
            const auto& event = mEvents[i];
            mProfileFile.write((char*)&event.mTimestampUs, sizeof(event.mTimestampUs));
            mProfileFile.write((char*)&event.mThreadId, sizeof(event.mThreadId));
            mProfileFile.write((char*)&event.mEventStringId, sizeof(event.mEventStringId));
            U64 eventType = (U64)event.mEventType;
            mProfileFile.write((char*)&eventType, sizeof(eventType));
        }

    }

    void ProfileContext::registerEvent(const SwChar* evt, ProfileEventType type)
    {
        if (mStringId.count(evt) == 0)
        {
            mStringId[evt] = mStringCount;
            mStringCount++;
        }

        auto& event = mEvents[mEventCount];
        mEventCount++;
        event.mTimestampUs = swizzle::core::GetCurrentTimeUs();
        event.mThreadId = swizzle::core::GetCurrentThreadId();
        event.mEventStringId = mStringId[evt];
        event.mEventType = type;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */
