#ifndef CONTEXT_HPP
#define CONTEXT_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/Api.hpp>
#include <swizzle/core/Window.hpp>

#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/FrameBuffer.hpp>
#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/gfx/Texture.hpp>
#include <swizzle/gfx/GfxDevice.hpp>

#include <swizzle/gfx/GfxStats.hpp>

/* Defines */

#define CONTEXT_SELECTED_DEVICE_NAME (~0u)

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::gfx
{
    struct GfxContextCreateInfo
    {
        SwBool mDebugMemory;
        SwBool mDebugCallbacks;
    };

    struct GfxContextInitializeInfo
    {
        U32 mDeviceIndex;

        U32 mNumWorkerQueues;
        U32 mNumTransferQueues;
    };

} // namespace swizzle::gfx

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
        virtual SwBool isDiscreteGpu(U32 deviceIndex) = 0;
        virtual common::Resource<GfxDevice> initializeDevice(const GfxContextInitializeInfo& createInfo) = 0;
    };

} // namespace swizzle::gfx

/* Function Declaration */

namespace swizzle::gfx
{
    common::Resource<GfxContext> SWIZZLE_API CreateContext(const GfxContextCreateInfo& createInfo);
}

#endif
