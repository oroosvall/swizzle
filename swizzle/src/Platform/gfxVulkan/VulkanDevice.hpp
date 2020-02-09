#ifndef VULKAN_DEVICE_HPP
#define VULKAN_DEVICE_HPP

#include "VulkanSwapchain.hpp"

#include "vk.hpp"

#include <string>
#include <vector>

#include <unordered_map>

namespace swizzle
{

	union Id
	{
		struct
		{
			uint32_t v1;
			uint32_t v2;
		} s;
		uint64_t v;
	};

    class LogicalDevice;

    class PhysicalDevice
    {
    public:
        PhysicalDevice(VkPhysicalDevice phys);

        std::vector<const char*> listAvailableExtensions();

        void activateExtension(const char* extension);

        uint32_t getNumQueues() const;

        /*
        @param queuePrios - contains a list of values between 0.0F - 1.0F
        values will be normalized before setting up the device;
        */
        LogicalDevice* createLogicalDevice(std::vector<float>& queuePrios);

    private:
        VkPhysicalDevice mPhysicalDevice;

        std::vector<const char*> mActiveExtensions;
    };

    class LogicalDevice
    {
    public:
        LogicalDevice(VkPhysicalDevice phys, const std::vector<const char*>& mExtensions,
                      std::vector<float>& queuePrios);

		virtual ~LogicalDevice();

        VulkanSwapchain* createSwapchain(VkSurfaceKHR mSurface, VulkanMemoryHelper& memhelper);

		VkPhysicalDevice getPhysical() const;
		VkDevice getLogical() const;
		VkCommandPool getCommandPool() const;

		VkQueue getQueue(uint32_t queueFamily, uint32_t queueIndex) const;

    private:
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mLogicalDevice;
		VkCommandPool mCommandPool;

		mutable std::unordered_map<uint64_t, VkQueue> mCahcedQueues;

    };

} // namespace renderer

#endif