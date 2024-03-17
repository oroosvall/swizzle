/* Include files */

#include "Ctx.hpp"

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
    Ctx::Ctx(bool debugCallbacks)
        : mVkInstance(nullptr)
    {
        //mVkInstance = std::make_shared<Instance>(debugCallbacks);
    }

    Ctx::~Ctx()
    {
        //mVkInstance.reset();
    }

    uint32_t Ctx::getDeviceCount() const
    {
        //return mVkInstance->getNumDevices();
        return false;
    }

    std::string Ctx::getDeviceName(uint32_t deviceIndex) const
    {
        // SWIZZLE_PROFILE_EVENT("VkGfxContext::getDeviceName");
        //return mVkInstance->getDeviceName(deviceIndex);
        return false;
    }

    bool Ctx::isDiscreteGpu(uint32_t deviceIndex) const
    {
        //return mVkInstance->isDiscreteGpu(deviceIndex);
        return false;
    }

    bool Ctx::hasFeatureSupport(uint32_t devIdx, const Features& features) const
    {
        //return mVkInstance->hasFeatureSupport(devIdx, features);
        return false;
    }

    std::shared_ptr<void> Ctx::createDevice(const DeviceCreateInfo& createInfo)
    {
        // std::vector<const char*> extensions;

        // auto supportedExtensions = mVkInstance->listAvailableExtensions(createInfo.mDeviceIndex);

        // addExtensionIfExisting(extensions, supportedExtensions, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
        // addExtensionIfExisting(extensions, supportedExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        // addExtensionIfExisting(extensions, supportedExtensions, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME);
        // addExtensionIfExisting(extensions, supportedExtensions, VK_KHR_MAINTENANCE_4_EXTENSION_NAME);

        // if (addExtensionIfExisting(extensions, supportedExtensions, VK_EXT_MESH_SHADER_EXTENSION_NAME))
        //{
        //     mMeshShaderSupported = true;
        // }

        // LOG_INFO("Initializing Vulkan Device: %s", getDeviceName(createInfo.mDeviceIndex));
        // common::Resource<vk::Device> dev = mVkInstance->initializeDevice(createInfo, extensions);
        //std::shared_ptr<int> dev = mVkInstance->createDevice(createInfo);

        // auto device = common::CreateRef<VkGfxDevice>(mVkInstance, dev);

        // return device;
        return nullptr;
    }

    bool Ctx::isValid() const
    {
        //return mVkInstance->isValid();
        return false;
    }

} // namespace rvk

/* Class Protected Function Definition */

/* Class Private Function Definition */
