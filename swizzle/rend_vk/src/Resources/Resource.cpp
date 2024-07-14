
/* Include files */

#include "Resource.hpp"

#include "Device.hpp"
#include "DeviceMemory.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace rvk
{
    void Resource<VkImage>::bindMemory(std::shared_ptr<Device> dev, DeviceMemory* mem)
    {
        mem->addUser();
        auto res = vkBindImageMemory(dev->getHandle(), mResource, mem->mMemory, mem->mAlignOffset);
        rvk::LogVulkanError(res, "vkBindImageMemory");
    }

    void Resource<VkBuffer>::bindMemory(std::shared_ptr<Device> dev, DeviceMemory* mem)
    {
        mem->addUser();
        auto res = vkBindBufferMemory(dev->getHandle(), mResource, mem->mMemory, mem->mAlignOffset);
        rvk::LogVulkanError(res, "vkBindBufferMemory");
    }
} // namespace rvk

/* Class Protected Function Definition */

/* Class Private Function Definition */
