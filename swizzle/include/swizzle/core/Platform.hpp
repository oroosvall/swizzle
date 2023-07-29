#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <swizzle/Api.hpp>
#include <common/Common.hpp>
#include <swizzle/core/Input.hpp>
#include <swizzle/core/File.hpp>

namespace swizzle::core
{
    // TODO: remove?
    U32 SWIZZLE_API GetNumberOfDisplays();
    U32 SWIZZLE_API GetDisplayResolutionCount(U32 displayIndex);
    void SWIZZLE_API GetDisplayResolutions(U32 displayIndex, Resolution* resolutions, U32 size);
    // TODO END

    const SwChar SWIZZLE_API * GetSaveGameDirectory();
    const SwChar SWIZZLE_API * GetTempDirectory();
    const SwChar SWIZZLE_API * GetAppCacheDirectory();

    U64 SWIZZLE_API GetCurrentTimeMs();
    U64 SWIZZLE_API GetCurrentTimeUs();
    U32 SWIZZLE_API GetCurrentThreadId();

    const SwChar SWIZZLE_API * GetKeyText(S32 key);
    const SwWChar SWIZZLE_API * GetKeyTextW(S32 key);

    S32 SWIZZLE_API KeyToScanCode(input::Keys);

    void SWIZZLE_API ShowCriticalMessage(const SwChar* text);
    void SWIZZLE_API ShowCriticalMessageW(const SwWChar* text);

    common::Resource<IFile> SWIZZLE_API OpenFile(const SwChar* path, FileMode mode);
    common::Resource<IFile> SWIZZLE_API OpenFileW(const SwWChar* path, FileMode mode);
}

#endif