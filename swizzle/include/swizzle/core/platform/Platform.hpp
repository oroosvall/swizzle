#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <swizzle/Api.hpp>
#include <swizzle/core/common/Types.hpp>
#include <swizzle/core/common/Resource.hpp>
#include <swizzle/input/Input.hpp>

#include "Window.hpp"
#include "File.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"

namespace swizzle::core
{

    U32 SWIZZLE_API GetNumberOfDisplays();
    U32 SWIZZLE_API GetDisplayResolutionCount(U32 displayIndex);
    void SWIZZLE_API GetDisplayResolutions(U32 displayIndex, Resolution* resolutions, U32 size);

    const SwChar SWIZZLE_API * GetSaveGameDirectory();
    const SwChar SWIZZLE_API * GetTempDirectory();
    const SwChar SWIZZLE_API * GetAppCacheDirectory();

    U64 SWIZZLE_API GetCurrentTimeMs();
    U32 SWIZZLE_API GetCurrentThreadId();

    Resource<Window> SWIZZLE_API CreateWindow(const U32 width, const U32 height, const SwChar* title);
    Resource<Thread> SWIZZLE_API CreateThread();

    const SwChar* GetKeyText(S32 key);
    const SwWChar* GetKeyTextW(S32 key);

    const S32 KeyToScanCode(input::Keys);

}

#endif