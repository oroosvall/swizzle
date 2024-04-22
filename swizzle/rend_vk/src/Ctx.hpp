
#pragma once

/* Include files */

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <rend_vk/Context.hpp>

#include "resources/Instance.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace rvk
{
    class Ctx : public Context
    {
    public:
        Ctx(bool debugCallbacks);
        virtual ~Ctx();

        virtual uint32_t getDeviceCount() const override;
        virtual std::string getDeviceName(uint32_t deviceIndex) const override;
        virtual bool isDiscreteGpu(uint32_t deviceIndex) const override;
        virtual bool hasFeatureSupport(uint32_t devIdx, const Features& features) const override;
        virtual std::shared_ptr<GfxDevice> createDevice(const DeviceCreateInfo& createInfo) override;

        bool isValid() const;

    private:

        std::shared_ptr<Instance> mVkInstance;
    };
} // namespace rvk

/* Function Declaration */
