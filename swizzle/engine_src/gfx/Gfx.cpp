
/* Include files */

#include <swizzle/core/Logging.hpp>
#include "Gfx.hpp"
#include "gfxvk/VkContext.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::gfx
{

    core::Resource<VulkanInstance> gVkInst;

    SwBool GfxInitialize()
    {
        LOG_INFO("Initializing Graphics!");
        gVkInst = core::CreateRef<VulkanInstance>();

        return true;
    }

    SwBool GfxCleanup()
    {
        LOG_INFO("Shutting down Graphics!");

        gVkInst.reset();

        return true;
    }

    core::Resource<GfxContext> CreateContext(const GfxContextCreateInfo& createInfo)
    {
        return core::CreateRef<VkGfxContext>(gVkInst, createInfo.mDeviceIndex);
    }

    U32 GetDeviceCount()
    {
        auto numDevices = gVkInst->listDevices().size();
        return static_cast<U32>(numDevices);
    }

    U32 GetPreferredDevice(U32 preferredDeviceIndex)
    {
        if (preferredDeviceIndex >= GetDeviceCount())
        {
            LOG_WARNING("Preferred device index out of range, selecting device with index 0\n");
            preferredDeviceIndex = 0U;
        }
        return preferredDeviceIndex;
    }

    const SwCharPtr GetDeviceName(U32 deviceIndex)
    {
        deviceIndex;
        LOG_WARNING("Not yet implemented\n");
        return "NULL";
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */