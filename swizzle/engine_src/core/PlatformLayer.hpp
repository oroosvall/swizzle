#ifndef PLATFORM_LAYER_HPP
#define PLATFORM_LAYER_HPP

#include <common/Common.hpp>

#include <swizzle/core/Platform.hpp>

namespace swizzle::core
{
    namespace platform
    {
        void platformInit(const SwChar* appName);
        void platformDeInit();

        const SwChar* getPlatformSaveGameDirectory();
        const SwChar* getPlatformTempDirectory();
        const SwChar* getPlatformAppCacheDirectory();

        U32 getMonitorCount();
        MonitorInfo getMonitorInfo(U32 index);

        // TODO: remove?
        U32 getPlatformDisplayCount();
        U32 getPlatformDisplayResolutionCount(U32 displayIndex);
        void getPlatformDisplayResolutions(U32 displayIndex, Resolution* resolutions, U32 size);
        // TODO END

        U64 getPlatformTimeStampMs();
        U64 getPlatformTimeStampUs();
        U32 getPlatformCurrentThreadId();

        common::Resource<SwWindow> createPlatformWindow(const U32 width, const U32  height, const SwChar* title);

        const SwChar* GetPlatformKeyText(S32 key);
        const SwWChar* GetPlatformKeyTextW(S32 key);

        S32 PlatformKeyToScanCode(input::Keys key);

        void PlatformShowCriticalMessage(const SwChar* text);
        void PlatformShowCriticalMessageW(const SwWChar* text);

        common::Resource<IFile> OpenFile(const SwChar* path, FileMode mode);
        common::Resource<IFile> OpenFileW(const SwWChar* path, FileMode mode);
    }

} // namespace swizzle

#endif