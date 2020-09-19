#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <swizzle/Api.hpp>
#include <swizzle/core/common/Types.hpp>
#include <swizzle/core/common/Resource.hpp>

#include "Window.hpp"
#include "File.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"

namespace swizzle::core
{

    SwU32 SWIZZLE_API GetNumberOfDisplays();
    SwU32 SWIZZLE_API GetDisplayResolutionCount(SwU32 displayIndex);
    void SWIZZLE_API GetDisplayResolutions(SwU32 displayIndex, Resolution* resolutions, SwU32 size);

    const SwChar SWIZZLE_API * GetSaveGameDirectory();
    const SwChar SWIZZLE_API * GetTempDirectory();
    const SwChar SWIZZLE_API * GetAppCacheDirectory();

    SwU64 SWIZZLE_API GetCurrentTimeMs();
    SwU32 SWIZZLE_API GetCurrentThreadId();


    Resource<Window> SWIZZLE_API CreateWindow(const SwU32 width, const SwU32 height, const SwChar* title);
    Resource<Thread> SWIZZLE_API CreateThread();

}

#endif