#ifndef VULKAN_PHYSICAL_DEVICE_2_HPP
#define VULKAN_PHYSICAL_DEVICE_2_HPP

/* Include files */

#include <common/Resource.hpp>
#include <common/Types.hpp>

#include "backend/Vk.hpp"

#include <string>
#include <vector>

#include <unordered_map>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

namespace swizzle
{
    class LogicalDevice;
}

/* Struct Declaration */

namespace swizzle
{
    union QueueId
    {
        struct
        {
            U32 v1;
            U32 v2;
        } s;
        U64 v;
    };
}

/* Class Declaration */

namespace swizzle
{
    class PhysicalDevice
    {
    public:
        PhysicalDevice(VkPhysicalDevice phys);

        std::vector<std::string> listAvailableExtensions();

        void activateExtension(const char* extension);

        U32 getNumQueues() const;

        /*
        @param queuePrios - contains a list of values between 0.0F - 1.0F
        values will be normalized before setting up the device;
        */
        core::Resource<LogicalDevice> createLogicalDevice(std::vector<F32>& queuePrios);

        VkPhysicalDeviceMemoryProperties getMemoryProperties() const;

    private:
        VkPhysicalDevice mPhysicalDevice;
        VkPhysicalDeviceMemoryProperties mMemoryProperties;

        std::vector<const char*> mActiveExtensions;
    };

    class LogicalDevice
    {
    public:
        LogicalDevice(VkPhysicalDevice phys, const std::vector<const char*>& mExtensions,
                      std::vector<F32>& queuePrios);

        virtual ~LogicalDevice();

        VkPhysicalDevice getPhysical() const;
        VkDevice getLogical() const;
        VkCommandPool getCommandPool() const;

        VkQueue getQueue(U32 queueFamily, U32 queueIndex) const;

        VkDevice operator()() const;

    private:
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mLogicalDevice;
        VkCommandPool mCommandPool;

        mutable std::unordered_map<U64, VkQueue> mCahcedQueues;
    };

} // namespace swizzle


/* Function Declaration */

#endif