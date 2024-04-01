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
        mVkInstance = std::make_shared<Instance>(debugCallbacks);
    }

    Ctx::~Ctx()
    {
        mVkInstance.reset();
    }

    uint32_t Ctx::getDeviceCount() const
    {
        return mVkInstance->getNumDevices();
    }

    std::string Ctx::getDeviceName(uint32_t deviceIndex) const
    {
        return mVkInstance->getDeviceName(deviceIndex);
    }

    bool Ctx::isDiscreteGpu(uint32_t deviceIndex) const
    {
        return mVkInstance->isDiscreteGpu(deviceIndex);
    }

    bool Ctx::hasFeatureSupport(uint32_t devIdx, const Features& features) const
    {
        return mVkInstance->hasFeatureSupport(devIdx, features);
    }

    std::shared_ptr<int> Ctx::createDevice(const DeviceCreateInfo& createInfo)
    {
        std::shared_ptr<int> dev = mVkInstance->createDevice(createInfo);

        // auto device = common::CreateRef<VkGfxDevice>(mVkInstance, dev);

        // return device;
        return dev;
    }

    bool Ctx::isValid() const
    {
        return mVkInstance->isValid();
    }

} // namespace rvk

/* Class Protected Function Definition */

/* Class Private Function Definition */
