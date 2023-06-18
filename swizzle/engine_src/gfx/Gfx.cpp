
/* Include files */

#include <common/Common.hpp>
#include <swizzle/core/Logging.hpp>
#include <swizzle/profiler/Profiler.hpp>

#include "Gfx.hpp"
#include "gfxvk2/VkContext.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::gfx
{

    //common::Resource<VulkanInstance> gVkInst = nullptr;

    SwBool GfxInitialize()
    {
        LOG_INFO("Initializing Graphics!");
        //gVkInst = common::CreateRef<VulkanInstance>();

        return true;
    }

    SwBool GfxCleanup()
    {
        LOG_INFO("Shutting down Graphics!");

        //gVkInst.reset();

        return true;
    }

    common::Resource<GfxContext> CreateContext(const GfxContextCreateInfo& createInfo)
    {
        auto res = common::CreateRef<VkGfxContext>(createInfo);
        return res;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */