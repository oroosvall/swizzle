#include <swizzle/core/Platform.hpp>
#include "PlatformLayer.hpp"

namespace swizzle::core
{

    U32 GetNumberOfDisplays()
    {
        return platform::getPlatformDisplayCount();
    }

    U32 GetDisplayResolutionCount(U32 displayIndex)
    {
        return platform::getPlatformDisplayResolutionCount(displayIndex);
    }

    void GetDisplayResolutions(U32 displayIndex, Resolution* resolutions, U32 size)
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

    U64 GetCurrentTimeMs()
    {
        return platform::getPlatformTimeStampMs();
    }

    U32 GetCurrentThreadId()
    {
        return platform::getPlatformCurrentThreadId();
    }

    Resource<Window> CreateWindow(const U32 width, const U32 height, const SwChar* title)
    {
        return platform::createPlatformWindow(width, height, title);
    }

    Resource<Thread> CreateThread()
    {
        return nullptr;
    }

    const SwCharPtr GetKeyText(S32 scanCode)
    {
        return platform::GetPlatformKeyText(scanCode);
    }

    const SwCharPtr GetKeyTextW(S32 scanCode)
    {
        return platform::GetPlatformKeyTextW(scanCode);
    }

    const S32 KeyToScanCode(input::Keys key)
    {
        return platform::PlatformKeyToScanCode(key);
    }


}