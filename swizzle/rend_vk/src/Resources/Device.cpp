/* Include files */

#include "Device.hpp"
#include "Extension.hpp"

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
    Device::Device(std::shared_ptr<Instance> instance,
                   VkPhysicalDevice physDev,
                   VkDevice logDev,
                   std::shared_ptr<Extensions> exts)
        : mInstance(instance)
        , mPhysicalDevice(physDev)
        , mLogicalDevice(logDev)
        , mExtensions(exts)
    {
    }

    Device::~Device()
    {
    }

    void Device::waitIdle() const
    {
        VkResult res = vkDeviceWaitIdle(mLogicalDevice);
        LogVulkanError(res, "vkDeviceWaitIdle");
    }

    std::string Device::getDeviceName() const
    {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &props);
        return props.deviceName;
    }

} // namespace rvk

/* Class Protected Function Definition */

/* Class Private Function Definition */