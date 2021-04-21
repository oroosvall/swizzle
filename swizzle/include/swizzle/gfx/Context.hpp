#ifndef CONTEXT_HPP
#define CONTEXT_HPP

/* Include files */

#include <swizzle/Api.hpp>
#include <common/Types.hpp>
#include <common/Resource.hpp>
#include <swizzle/core/Window.hpp>

#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/gfx/Texture.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::gfx
{
    struct GfxStatistics
    {
        U64 mGpuMemoryUsage;
        U64 mCpuMemoryUsage;
        U32 mStagedObjects;
        U32 mNumTextures;
        U32 mNumBuffers;
        U32 mFrameTimeUs;
    };

    struct GfxContextCreateInfo
    {
        U32 mDeviceIndex;
        SwBool mDebugMemory;
    };
}

/* Class Declaration */

namespace swizzle::gfx
{
    class GfxContext
    {
    public:
        virtual ~GfxContext() {}

        virtual void waitIdle() = 0;

        virtual GfxStatistics getStatistics() = 0;

        virtual void resetFrameCount() = 0;
        virtual U32 getFrameCount() = 0;

        virtual core::Resource<Buffer> createBuffer(BufferType type) = 0;
        virtual core::Resource<CommandBuffer> createCommandBuffer(U32 swapCount) = 0;
        virtual core::Resource<Swapchain> createSwapchain(core::Resource<core::Window> window, U32 swapCount) = 0;
        virtual core::Resource<Texture> createTexture(U32 width, U32 height, U32 channels, const U8* data = nullptr) = 0;
        virtual core::Resource<Texture> createCubeMapTexture(U32 width, U32 height, U32 channels, const U8* data = nullptr) = 0;

    };
}

/* Function Declaration */

namespace swizzle::gfx
{
    core::Resource<GfxContext> SWIZZLE_API CreateContext(const GfxContextCreateInfo& createInfo);
    U32 SWIZZLE_API GetDeviceCount();
    U32 SWIZZLE_API GetPreferredDevice(U32 preferredDeviceIndex);
    const SwCharPtr SWIZZLE_API GetDeviceName(U32 deviceIndex);
}

#endif
