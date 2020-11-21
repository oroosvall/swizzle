#include "..\include\utils\FpsCounter.hpp"

namespace utils
{
    FpsCounter::FpsCounter() noexcept
        : mFps(0U)
        , mCount(0U)
        , mTime(0.0F)
    {
    }

    void FpsCounter::tick(float dt) noexcept
    {
        mCount++;
        mTime += dt;
        if (mTime > 1.0F)
        {
            mFps = mCount;
            mCount = 0;
            mTime -= 1.0F;
        }
    }

    uint32_t FpsCounter::getFps() const noexcept
    {
        return mFps;
    }
} // namespace utils