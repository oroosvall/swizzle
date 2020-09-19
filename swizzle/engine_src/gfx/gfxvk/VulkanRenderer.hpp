#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include "gfx/Gfx.hpp"

#include "vk.hpp"
#include "VulkanInstance.hpp"
#include "VulkanObjectContainer.hpp"

namespace swizzle::gfx
{
    class VulkanRenderer
    {
    public:
        VulkanRenderer();

        virtual ~VulkanRenderer();

        const VulkanObjectContainer& getVkObjects();

    private:
        VulkanInstance* mInstance;
        PhysicalDevice* mPhysical;
        LogicalDevice* mLogical;

        VulkanObjectContainer mVkObjects;

    };

} // namespace swizzle::gfx

#endif