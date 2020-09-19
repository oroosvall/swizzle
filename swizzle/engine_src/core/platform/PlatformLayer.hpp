#ifndef PLATFORM_LAYER_HPP
#define PLATFORM_LAYER_HPP

#include <swizzle/core/common/Types.hpp>
#include <swizzle/core/common/Resource.hpp>

#include <swizzle/core/platform/Platform.hpp>

//#include <swizzle/Renderer.hpp>
//#include <swizzle/Window.hpp>

namespace swizzle::core
{
    namespace platform
    {
        void platformInit(const SwChar* appName);
        void platformDeInit();

        const SwChar* getPlatformSaveGameDirectory();
        const SwChar* getPlatformTempDirectory();
        const SwChar* getPlatformAppCacheDirectory();

        SwU32 getPlatformDisplayCount();
        SwU32 getPlatformDisplayResolutionCount(SwU32 displayIndex);
        void getPlatformDisplayResolutions(SwU32 displayIndex, Resolution* resolutions, SwU32 size);


        const SwU64 getPlatformTimeStampMs();
        const SwU32 getPlatformCurrentThreadId();

        Resource<Window> createPlatformWindow(const SwU32 width, const SwU32  height, const SwChar* title);
    }

    //void getPlatformDisplayResolution(uint32_t displayIndex, uint32_t& resolutionCount, Resolution* resolutions);


    //Renderer* platformGfxVulkan(Window* window);
    //Renderer* platformGfxOpenGl(Window* window);

} // namespace swizzle

#endif