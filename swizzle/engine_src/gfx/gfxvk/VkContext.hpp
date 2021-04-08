#ifndef VK_CONTEXT_HPP
#define VK_CONTEXT_HPP

/* Include files */

#include <common/Types.hpp>
#include <common/Resource.hpp>
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

        VkGfxContext(core::Resource<VulkanInstance> vkInstance, U32 deviceIndex);
        virtual ~VkGfxContext();

        virtual void waitIdle() override;

        virtual GfxStatistics getStatistics() override;

        virtual void resetFrameCount() override;
        virtual U32 getFrameCount() override;

        virtual core::Resource<Buffer> createBuffer(BufferType type) override;
        virtual core::Resource<CommandBuffer> createCommandBuffer(U32 swapCount) override;
        virtual core::Resource<Swapchain> createSwapchain(core::Resource<core::Window> window, U32 swapCount) override;
        virtual core::Resource<Texture> createTexture(U32 width, U32 height) override;

        VkContainer getVkContainer() const;

    private:

        core::Resource<VulkanInstance> mVkInstance;

        core::Resource<PhysicalDevice> mPhysicalDevice;
        core::Resource<LogicalDevice> mLogicalDevice;

        VkContainer mVkContainer;

    };
}

/* Function Declaration */

#endif