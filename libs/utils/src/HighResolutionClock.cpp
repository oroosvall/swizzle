#include <utils/HighResolutionClock.hpp>

namespace utils
{
    HighResolutionClock::HighResolutionClock()
        : mTime(std::chrono::high_resolution_clock::now())
    {
    }

    void HighResolutionClock::reset()
    {
        mTime = std::chrono::high_resolution_clock::now();
    }

    uint32_t HighResolutionClock::seconds(bool reset)
    {

        auto now = std::chrono::high_resolution_clock::now();
        auto diff = now - mTime;

        if (reset)
        {
            mTime = now;
        }

        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(diff).count());
    }

    uint32_t HighResolutionClock::milliseconds(bool reset)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto diff = now - mTime;

        if (reset)
        {
            mTime = now;
        }

        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
    }

    float_t HighResolutionClock::secondsAsFloat(bool reset)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto diff = now - mTime;

        if (reset)
        {
            mTime = now;
        }

        auto time = std::chrono::duration_cast<std::chrono::duration<float>>(diff);

        return time.count();
    }

} // namespace utils