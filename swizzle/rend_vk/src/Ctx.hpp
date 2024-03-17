
#pragma once

/* Include files */

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <rend_vk/Context.hpp>

//#include "resources/Instance.hpp"

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
        virtual std::shared_ptr<void> createDevice(const DeviceCreateInfo& createInfo) override;

        bool isValid() const;

    private:
        bool addExtensionIfExisting(std::vector<const char*>& extensions,
                                    const std::vector<std::string>& availableExtensions, const char* extensionName);

        //std::shared_ptr<Instance> mVkInstance;
        void* mVkInstance;
    };
} // namespace rvk

/* Function Declaration */
