#ifndef VULKAN_INSTANCE_HPP
#define VULKAN_INSTANCE_HPP

#include "backend/Vk.hpp"
//#include <swizzle/Window.hpp>

#include "VulkanPhysicalDevice.hpp"
#include <vector>

namespace swizzle::gfx
{
    class VulkanInstance
    {

    public:
        VulkanInstance();
        ~VulkanInstance();

        const VkInstance getInstance() const;

        std::vector<PhysicalDevice> listDevices();

    private:
        void createInstance();
        void initDebug();
        void deinitDebug();

        VkInstance mVkInstance;
        VkSurfaceKHR mSurface;

        VkDebugReportCallbackEXT mDebugCallback;

        PFN_vkCreateDebugReportCallbackEXT mCreateDebugCallback;
        PFN_vkDestroyDebugReportCallbackEXT mDestroyDebugCallback;
    };
}

#endif
