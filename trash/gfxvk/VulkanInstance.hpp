#ifndef VULKAN_INSTANCE_HPP
#define VULKAN_INSTANCE_HPP

#include "backend/Vk.hpp"

#include "VulkanDevice.hpp"
#include <vector>

namespace swizzle::gfx
{
    class VulkanInstance
    {

    public:
        VulkanInstance();
        ~VulkanInstance();

        VkInstance getInstance() const;

        std::vector<PhysicalDevice> listDevices();

    private:
        void createInstance();
        void initDebug();
        void deinitDebug();

        void addLayerIfExisting(std::vector<const char*>& layers, const std::vector<VkLayerProperties>& availableLayers,
                                const char* layerName);

        VkInstance mVkInstance;

        VkDebugReportCallbackEXT mDebugCallback;

        PFN_vkCreateDebugReportCallbackEXT mCreateDebugCallback;
        PFN_vkDestroyDebugReportCallbackEXT mDestroyDebugCallback;
    };
}

#endif
