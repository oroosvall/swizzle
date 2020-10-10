
#include "VulkanMemoryHelper.hpp"

namespace swizzle
{
	VulkanMemoryHelper::VulkanMemoryHelper(VkPhysicalDevice physicalDev)
	{
		vkGetPhysicalDeviceMemoryProperties(physicalDev, &mMemoryProperties);
	}

	VulkanMemoryHelper::~VulkanMemoryHelper() {}

	uint32_t VulkanMemoryHelper::findMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits) const
	{

		uint32_t result = 0xFFFFFFFF; // Unable to find memoryType


		for (uint32_t i = 0; i < mMemoryProperties.memoryTypeCount; i++)
		{
			if ((mMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
			{
				result = i;
				break;
			}
		}
		return result;
	}
} // namespace renderApi