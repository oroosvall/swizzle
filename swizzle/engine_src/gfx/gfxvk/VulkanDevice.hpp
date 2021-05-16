#ifndef VULKAN_DEVICE_HPP
#define VULKAN_DEVICE_HPP

/* Include files */

#include <common/Resource.hpp>
#include <common/Types.hpp>

#include "backend/Vk.hpp"

#include <string>
#include <vector>

#include <unordered_map>
#include <memory>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

namespace swizzle
{
    namespace gfx
    {
        class VkBufferInt;
    }
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

    class LogicalDevice : public std::enable_shared_from_this<LogicalDevice>
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

        // Resource creation
        std::shared_ptr<gfx::VkBufferInt> createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags);
        void destroyBuffer(VkBuffer buffer);

        vk::VkOwner<VkDeviceMemory> allocateMemory(VkDeviceSize size, U32 memoryIndex);
        void dequeFreeMemory(vk::VkOwner<VkDeviceMemory> buffer);

        // Statistics information

        U32 getNumBuffers() { return mNumBuffers; }
        U32 getNumTextures() { return mNumTextures; }

    private:
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mLogicalDevice;
        VkCommandPool mCommandPool;

        mutable std::unordered_map<U64, VkQueue> mCahcedQueues;

        U32 mNumBuffers = 0u;
        U32 mNumTextures = 0u;
    };

} // namespace swizzle


/* Function Declaration */

#endif