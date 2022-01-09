#ifndef VK_CONTEXT_HPP
#define VK_CONTEXT_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

#include "VulkanInstance.hpp"
#include "backend/VkContainer.hpp"

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

        VkGfxContext(common::Resource<VulkanInstance> vkInstance, U32 deviceIndex);
        virtual ~VkGfxContext();

        virtual void waitIdle() override;

        virtual GfxStatistics getStatistics() override;

        virtual common::Resource<Buffer> createBuffer(BufferType type) override;
        virtual common::Resource<CommandBuffer> createCommandBuffer(U32 swapCount) override;
        virtual common::Resource<Swapchain> createSwapchain(common::Resource<core::SwWindow> window, U32 swapCount) override;
        virtual common::Resource<Texture> createTexture(U32 width, U32 height, U32 channels, const U8* data = nullptr) override;
        virtual common::Resource<Texture> createCubeMapTexture(U32 width, U32 height, U32 channels, const U8* data = nullptr) override;

        virtual const SwChar* getDeviceName() const override;

        VkContainer getVkContainer() const;

    private:

        common::Resource<VulkanInstance> mVkInstance;

        common::Resource<PhysicalDevice> mPhysicalDevice;
        common::Resource<LogicalDevice> mLogicalDevice;

        VkContainer mVkContainer;

    };

}

/* Function Declaration */

#endif