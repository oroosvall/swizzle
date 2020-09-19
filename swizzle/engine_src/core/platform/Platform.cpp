#include <swizzle/core/platform/Platform.hpp>
#include "PlatformLayer.hpp"

namespace swizzle::core
{

    SwU32 GetNumberOfDisplays()
    {
        return platform::getPlatformDisplayCount();
    }

    SwU32 GetDisplayResolutionCount(SwU32 displayIndex)
    {
        return platform::getPlatformDisplayResolutionCount(displayIndex);
    }

    void GetDisplayResolutions(SwU32 displayIndex, Resolution* resolutions, SwU32 size)
    {
        platform::getPlatformDisplayResolutions(displayIndex, resolutions, size);
    }

    const SwChar* GetSaveGameDirectory()
    {
        return platform::getPlatformSaveGameDirectory();
    }

    const SwChar* GetTempDirectory()
    {
        return platform::getPlatformTempDirectory();
    }

    const SwChar* GetAppCacheDirectory()
    {
        return platform::getPlatformAppCacheDirectory();
    }

    SwU64 GetCurrentTimeMs()
    {
        return platform::getPlatformTimeStampMs();
    }

    SwU32 GetCurrentThreadId()
    {
        return platform::getPlatformCurrentThreadId();
    }

    Resource<Window> CreateWindow(const SwU32 width, const SwU32 height, const SwChar* title)
    {
        return platform::createPlatformWindow(width, height, title);
    }

    Resource<Thread> CreateThread()
    {
        return nullptr;
    }

}