#ifndef VULKAN_MEMORY_HELPER_HPP
#define VULKAN_MEMORY_HELPER_HPP

#include "vk.hpp"

namespace swizzle
{

	class VulkanMemoryHelper
	{
	public:
		VulkanMemoryHelper(VkPhysicalDevice physical);
		~VulkanMemoryHelper();

		uint32_t findMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits) const;

	private:
		VkPhysicalDeviceMemoryProperties mMemoryProperties;
	};

}


#endif