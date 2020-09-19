
#ifndef HIGH_RESOLUTION_CLOCK_HPP
#define HIGH_RESOLUTION_CLOCK_HPP

#include <chrono>

namespace utils
{

    class HighResolutionClock
    {
    public:
        HighResolutionClock();

        void reset();

        uint32_t seconds(bool reset = false);
        uint32_t milliseconds(bool reset = false);

        float_t secondsAsFloat(bool reset = false);

    private:
        std::chrono::high_resolution_clock::time_point mTime;
    };

} // namespace utils

#endif