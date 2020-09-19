#ifndef FPS_COUNTER_HPP
#define FPS_COUNTER_HPP

#include <cstdint>

namespace utils
{
    class FpsCounter
    {
    public:
        FpsCounter() noexcept;

        void tick(float dt) noexcept;
        uint32_t getFps() const noexcept;

    private:
        uint32_t mFps;
        uint32_t mCount;

        float mTime;
    };
} // namespace utils

#endif