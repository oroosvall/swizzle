#ifndef PLATFORM_LAYER_HPP
#define PLATFORM_LAYER_HPP

#include <swizzle/core/Types.hpp>
#include <swizzle/core/Resource.hpp>

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

        U32 getPlatformDisplayCount();
        U32 getPlatformDisplayResolutionCount(U32 displayIndex);
        void getPlatformDisplayResolutions(U32 displayIndex, Resolution* resolutions, U32 size);


        const U64 getPlatformTimeStampMs();
        const U32 getPlatformCurrentThreadId();

        Resource<Window> createPlatformWindow(const U32 width, const U32  height, const SwChar* title);

        const SwChar* GetPlatformKeyText(S32 key);
        const SwWChar* GetPlatformKeyTextW(S32 key);

        const S32 PlatformKeyToScanCode(input::Keys key);

    }

} // namespace swizzle

#endif