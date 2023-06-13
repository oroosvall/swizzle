#ifndef VK_CONTEXT_HPP
#define VK_CONTEXT_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/GfxDevice.hpp>
#include <swizzle/gfx/Context.hpp>

#include "res/CmdPool.hpp"
#include "res/Device.hpp"
#include "res/Fence.hpp"
#include "res/Instance.hpp"

#include <string>
#include <vector>

#include <utils/FrameAllocator.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gfx
{
    class VkGfxContext : public GfxContext
    {
    public:
        VkGfxContext(const GfxContextCreateInfo& createInfo);
        virtual ~VkGfxContext();

        virtual U32 getDeviceCount() override;
        virtual const SwChar* getDeviceName(U32 deviceIndex) override;
        virtual const SwChar* getSelectedDeviceName() override;
        virtual SwBool isDiscreteGpu(U32 deviceIndex) override;
        virtual common::Resource<GfxDevice> initializeDevice(const GfxContextInitializeInfo& createInfo) override;

    private:
        SwBool addExtensionIfExisting(std::vector<const char*>& extensions,
                                      const std::vector<std::string>& availableExtensions, const char* extensionName);

        common::Resource<vk::Instance> mVkInstance;
        common::Resource<vk::Device> mVkDevice;
        common::Resource<vk::CmdPool> mCmdPool;

        VkQueue mQueue;

        utils::FrameAllocator mFrameAllocator;
        std::vector<common::Resource<vk::Fence>> mFences;

        SwBool mMeshShaderSupported;
    };

} // namespace swizzle::gfx

/* Function Declaration */

#endif