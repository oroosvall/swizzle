#ifndef CONTEXT_HPP
#define CONTEXT_HPP

/* Include files */

#include <swizzle/Api.hpp>
#include <common/Common.hpp>
#include <swizzle/core/Window.hpp>

#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/gfx/Texture.hpp>


#define CONTEXT_SELECTED_DEVICE_NAME (~0u)
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
    };

    struct GfxContextCreateInfo
    {
        U32 mDeviceIndex;
        SwBool mDebugMemory;
        SwBool mDebugCallbacks;

        U32 mNumWorkerQueues;
        U32 mNumTransferQueues;
    };

    struct GfxContextInitializeInfo
    {
        U32 mDeviceIndex;

        U32 mNumWorkerQueues;
        U32 mNumTransferQueues;
    };

}

/* Class Declaration */

namespace swizzle::gfx
{
    class GfxContext
    {
    public:
        virtual ~GfxContext() {}

        virtual U32 getDeviceCount() = 0;
        virtual const SwChar* getDeviceName(U32 deviceIndex) = 0;
        virtual const SwChar* getSelectedDeviceName() = 0;
        virtual SwBool initializeDevice(const GfxContextInitializeInfo& createInfo) = 0;

        // All functions below must only be called after a successful call to
        // initializeDevice(...)
        virtual void waitIdle() = 0;

        virtual GfxStatistics getStatistics() = 0;

        virtual common::Resource<Buffer> createBuffer(BufferType type) = 0;
        virtual common::Resource<CommandBuffer> createCommandBuffer(U32 swapCount) = 0;
        virtual common::Resource<Swapchain> createSwapchain(common::Resource<core::SwWindow> window, U32 swapCount) = 0;
        virtual common::Resource<Texture> createTexture(U32 width, U32 height, U32 channels, const U8* data = nullptr) = 0;
        virtual common::Resource<Texture> createCubeMapTexture(U32 width, U32 height, U32 channels, const U8* data = nullptr) = 0;

        virtual void submit(common::Resource<CommandBuffer>* cmdBuffer, U32 cmdBufferCount,
                            common::Resource<Swapchain> swapchain) = 0;

    };

}

/* Function Declaration */

namespace swizzle::gfx
{
    common::Resource<GfxContext> SWIZZLE_API CreateContext(const GfxContextCreateInfo& createInfo);
    U32 SWIZZLE_API GetDeviceCount();
    U32 SWIZZLE_API GetPreferredDevice(U32 preferredDeviceIndex);
    const SwChar SWIZZLE_API * GetDeviceName(U32 deviceIndex);
}

#endif
