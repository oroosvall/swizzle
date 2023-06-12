#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <swizzle/Api.hpp>
#include <common/Common.hpp>
#include <swizzle/core/Input.hpp>

#include "Window.hpp"

namespace swizzle::core
{

    U32 SWIZZLE_API GetNumberOfDisplays();
    U32 SWIZZLE_API GetDisplayResolutionCount(U32 displayIndex);
    void SWIZZLE_API GetDisplayResolutions(U32 displayIndex, Resolution* resolutions, U32 size);

    const SwChar SWIZZLE_API * GetSaveGameDirectory();
    const SwChar SWIZZLE_API * GetTempDirectory();
    const SwChar SWIZZLE_API * GetAppCacheDirectory();

    U64 SWIZZLE_API GetCurrentTimeMs();
    U64 SWIZZLE_API GetCurrentTimeUs();
    U32 SWIZZLE_API GetCurrentThreadId();

    common::Resource<SwWindow> SWIZZLE_API CreateSwWindow(const U32 width, const U32 height, const SwChar* title);

    const SwChar SWIZZLE_API * GetKeyText(S32 key);
    const SwWChar SWIZZLE_API * GetKeyTextW(S32 key);

    S32 SWIZZLE_API KeyToScanCode(input::Keys);

    void SWIZZLE_API ShowCriticalMessage(const SwChar* text);
    void SWIZZLE_API ShowCriticalMessageW(const SwWChar* text);

}

#endif