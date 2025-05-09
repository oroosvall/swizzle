#include "PlatformLayer.hpp"
#include <swizzle/core/Platform.hpp>

namespace swizzle::core
{
    U32 GetMonitorCount()
    {
        return platform::getMonitorCount();
    }

    MonitorInfo GetMonitorInfo(U32 index)
    {
        return platform::getMonitorInfo(index);
    }

    // TODO: remove?
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

    // TODO END

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

    U64 GetCurrentTimeUs()
    {
        return platform::getPlatformTimeStampUs();
    }

    U32 GetCurrentThreadId()
    {
        return platform::getPlatformCurrentThreadId();
    }

    common::Resource<SwWindow> CreateSwWindow(const U32 width, const U32 height, const SwChar* title)
    {
        return platform::createPlatformWindow(width, height, title);
    }

    const SwChar* GetKeyText(S32 scanCode)
    {
        return platform::GetPlatformKeyText(scanCode);
    }

    const SwWChar* GetKeyTextW(S32 scanCode)
    {
        return platform::GetPlatformKeyTextW(scanCode);
    }

    S32 KeyToScanCode(input::Keys key)
    {
        return platform::PlatformKeyToScanCode(key);
    }

    void ShowCriticalMessage(const SwChar* text)
    {
        platform::PlatformShowCriticalMessage(text);
    }

    void ShowCriticalMessageW(const SwWChar* text)
    {
        platform::PlatformShowCriticalMessageW(text);
    }

    common::Resource<IFile> OpenFile(const SwChar* path, FileMode mode)
    {
        return platform::OpenFile(path, mode);
    }

    common::Resource<IFile> OpenFileW(const SwWChar* path, FileMode mode)
    {
        return platform::OpenFileW(path, mode);
    }

} // namespace swizzle::core