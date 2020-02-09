#ifndef VULKAN_INSTANCE_HPP
#define VULKAN_INSTANCE_HPP

#include <swizzle/Window.hpp>
#include "vk.hpp"

#include <vector>
#include "VulkanDevice.hpp"

namespace swizzle
{

    class VulkanInstance
    {

    public:
        VulkanInstance(Window* window);
        ~VulkanInstance();

        VkSurfaceKHR getSurface() const;

		std::vector<PhysicalDevice> listDevices();

    private:
        void createInstance();
        void createSurface(Window* window);
        void initDebug();
        void deinitDebug();

        VkInstance mVkInstance;
        VkSurfaceKHR mSurface;

        VkDebugReportCallbackEXT mDebugCallback;

        PFN_vkCreateDebugReportCallbackEXT mCreateDebugCallback;
        PFN_vkDestroyDebugReportCallbackEXT mDestroyDebugCallback;
    };

} // namespace renderer

#endif