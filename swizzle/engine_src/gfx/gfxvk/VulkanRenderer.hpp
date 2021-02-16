#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include "gfx/Gfx.hpp"

#include "backend/Vk.hpp"
#include "VulkanInstance.hpp"
#include "backend/VulkanObjectContainer.hpp"

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