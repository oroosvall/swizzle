
#pragma once

/* Include files */

#include "Vk.hpp"
#include "_fw.hpp"

#include <memory>
#include <string>
#include <vector>

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

        void initAfterConstruction();

        void waitIdle() const;
        std::string getDeviceName() const;

        std::shared_ptr<DeviceMemory> allocateMemory(VkMemoryPropertyFlags propertyFlags, VkMemoryRequirements req);
        void freeMemory(std::shared_ptr<DeviceMemory> memory);

        VkDevice getHandle() const;

        VkAllocationCallbacks* getAllocCallbacks() const;

    private:

        void initMemoryPools();

        std::shared_ptr<Instance> mInstance;
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mLogicalDevice;
        std::shared_ptr<Extensions> mExtensions;

        /// Memory stuff
        VkPhysicalDeviceMemoryProperties mMemoryProperties;
        std::vector<std::shared_ptr<DeviceMemoryPool>> mMemoryPools;
    };
} // namespace rvk

/* Function Declaration */
