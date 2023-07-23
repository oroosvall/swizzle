
/* Include files */

#include <swizzle/core/Logging.hpp>
#include <swizzle/profiler/Profiler.hpp>

#include "VkContext.hpp"
#include "VkDevice.hpp"
#include "Stats.hpp"

#include "res/DBuffer.hpp"
#include "res/VSwapchain.hpp"
#include "res/Texture2D.hpp"
#include "res/TextureCube.hpp"
#include "res/VFrameBuffer.hpp"
#include "res/VMaterial.hpp"
#include "res/ShaderPipeline.hpp"
#include "res/QueryPool.hpp"


//#include "VulkanBuffer.hpp"
//#include "VulkanSwapchain.hpp"
//#include "VulkanCmdBuffer.hpp"
//
//#include "VulkanCubeMap.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle::gfx
{
    VkGfxContext::VkGfxContext(const GfxContextCreateInfo& createInfo)
        : mVkInstance(nullptr)
        , mFrameAllocator(sizeof(void*) * 128u)
        , mMeshShaderSupported(false)
    {
        mVkInstance = common::CreateRef<vk::Instance>(createInfo);
    }

    VkGfxContext::~VkGfxContext()
    {
        mVkInstance.reset();
    }

    U32 VkGfxContext::getDeviceCount()
    {
        return mVkInstance->getNumDevices();
    }

    const SwChar* VkGfxContext::getDeviceName(U32 deviceIndex)
    {
        SWIZZLE_PROFILE_EVENT("VkGfxContext::getDeviceName");
        return mVkInstance->getDeviceName(deviceIndex);
    }

    SwBool VkGfxContext::isDiscreteGpu(U32 deviceIndex)
    {
        return mVkInstance->isDiscreteGpu(deviceIndex);
    }


    common::Resource<GfxDevice> VkGfxContext::initializeDevice(const GfxContextInitializeInfo& createInfo)
    {
        std::vector<const char*> extensions;

        auto supportedExtensions = mVkInstance->listAvailableExtensions(createInfo.mDeviceIndex);

        addExtensionIfExisting(extensions, supportedExtensions, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
        addExtensionIfExisting(extensions, supportedExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        addExtensionIfExisting(extensions, supportedExtensions, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME);
        addExtensionIfExisting(extensions, supportedExtensions, VK_KHR_MAINTENANCE_4_EXTENSION_NAME);

        if (addExtensionIfExisting(extensions, supportedExtensions, VK_EXT_MESH_SHADER_EXTENSION_NAME))
        {
            mMeshShaderSupported = true;
        }

        LOG_INFO("Initializing Vulkan Device: %s", getDeviceName(createInfo.mDeviceIndex));
        common::Resource<vk::Device> dev = mVkInstance->initializeDevice(createInfo, extensions);

        auto device = common::CreateRef<VkGfxDevice>(mVkInstance, dev);

        return device;
    }
}
/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace swizzle::gfx
{
    SwBool VkGfxContext::addExtensionIfExisting(std::vector<const char*>& extensions,
                                               const std::vector<std::string>& availableExtensions,
                                               const char* extensionName)
    {
        for (const auto& ext : availableExtensions)
        {
            if (ext == extensionName)
            {
                extensions.emplace_back(extensionName);
                return true;
            }
        }
        return false;
    }
}
