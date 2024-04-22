
#pragma once

/* Include files */

#include "Vk.hpp"
#include "_fw.hpp"

#include <memory>
#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace rvk
{
    class Device : public std::enable_shared_from_this<Device>
    {
    public:
        Device(std::shared_ptr<Instance> inst,
               VkPhysicalDevice physDev,
               VkDevice logDev,
               std::shared_ptr<Extensions> exts);
        virtual ~Device();

        void waitIdle() const;
        std::string getDeviceName() const;

    private:
        std::shared_ptr<Instance> mInstance;
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mLogicalDevice;
        std::shared_ptr<Extensions> mExtensions;
    };
} // namespace rvk

/* Function Declaration */
