#include "VulkanDevice.hpp"
#include <swizzle/core/Logging.hpp>

#include "VulkanBuffer.hpp"

namespace swizzle
{

    PhysicalDevice::PhysicalDevice(VkPhysicalDevice phys)
        : mPhysicalDevice(phys)
        , mMemoryProperties()
        , mActiveExtensions()
    {
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties);
    }

    std::vector<std::string> PhysicalDevice::listAvailableExtensions()
    {

        std::vector<std::string> extensions;

        U32 count = 0u;
        vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> exts;
        exts.resize(count);
        vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &count, exts.data());

        for (const auto& it : exts)
        {
            extensions.push_back(it.extensionName);
        }

        return extensions;
    }

    void PhysicalDevice::activateExtension(const char* extension)
    {
        mActiveExtensions.push_back(extension);
    }

    U32 PhysicalDevice::getNumQueues() const
    {
        U32 queueCount = 0u;

        U32 queueFamilyCount = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueProperties;
        queueProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueProperties.data());

        for (U32 i = 0u; i < queueFamilyCount; i++)
        {
            if ((queueProperties[i].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) ==
                VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
            {
                // logging::logInfo("Found Queue!\n");
                // mQueueFamilyIndex = i;
                queueCount = queueProperties[i].queueCount;
                break;
            }
        }

        return queueCount;
    }

    core::Resource<LogicalDevice> PhysicalDevice::createLogicalDevice(std::vector<F32>& queuPrio)
    {
        return core::CreateRef<LogicalDevice>(mPhysicalDevice, mActiveExtensions, queuPrio);
    }

    VkPhysicalDeviceMemoryProperties PhysicalDevice::getMemoryProperties() const
    {
        return mMemoryProperties;
    }

    LogicalDevice::LogicalDevice(VkPhysicalDevice phys, const std::vector<const char*>& extensions,
                                 std::vector<F32>& queuePrio)
    {
        mPhysicalDevice = phys;
        F32* prios = queuePrio.data();

        U32 queueFamilyIndex = 0u;

        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.queueCount = static_cast<U32>(queuePrio.size());
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.pQueuePriorities = prios;
        queueCreateInfo.flags = 0u;

        VkDeviceCreateInfo createInfo {};
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.enabledExtensionCount = static_cast<U32>(extensions.size());
        createInfo.enabledLayerCount = 0u;

        createInfo.flags = 0u;
        createInfo.pEnabledFeatures = nullptr;
        createInfo.pNext = nullptr;
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1u;

        VkResult res = vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("vkCreateDevice failed!, returned %s\n", vk::VkResultToString(res));
        }

        VkCommandPoolCreateInfo cmdPoolCreateInfo;
        cmdPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.pNext = VK_NULL_HANDLE;
        cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmdPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

        vkCreateCommandPool(mLogicalDevice, &cmdPoolCreateInfo, nullptr, &mCommandPool);
    }

    LogicalDevice::~LogicalDevice()
    {
        vkDestroyCommandPool(mLogicalDevice, mCommandPool, nullptr);
        vkDestroyDevice(mLogicalDevice, nullptr);
    }

    VkPhysicalDevice LogicalDevice::getPhysical() const
    {
        return mPhysicalDevice;
    }

    VkDevice LogicalDevice::getLogical() const
    {
        return mLogicalDevice;
    }

    VkCommandPool LogicalDevice::getCommandPool() const
    {
        return mCommandPool;
    }

    VkQueue LogicalDevice::getQueue(U32 queueFamily, U32 queueIndex) const
    {
        VkQueue queue = VK_NULL_HANDLE;

        QueueId v = {};
        v.s.v1 = queueFamily;
        v.s.v2 = queueIndex;

        auto elem = mCahcedQueues.find(v.v);
        if (elem != mCahcedQueues.end())
        {
            queue = elem->second;
        }
        else
        {
            vkGetDeviceQueue(mLogicalDevice, queueFamily, queueIndex, &queue);
            mCahcedQueues[v.v] = queue;
        }
        return queue;
    }

    VkDevice LogicalDevice::operator()(void) const
    {
        return mLogicalDevice;
    }

    std::shared_ptr<gfx::VkBufferInt> LogicalDevice::createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags)
    {
        VkBuffer buf = VK_NULL_HANDLE;

        VkBufferCreateInfo createInfo = {};

        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0u;
        createInfo.size = size;
        createInfo.usage = usageFlags;
        createInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0u;           // This is 0 because of EXCLUSIVE ACCESS
        createInfo.pQueueFamilyIndices = VK_NULL_HANDLE; // This is 0 because of EXCLUSIVE ACCESS

        vkCreateBuffer(mLogicalDevice, &createInfo, nullptr, &buf);
        mNumBuffers++;
        auto device = shared_from_this();

        return std::make_shared<gfx::VkBufferInt>(device, buf );
    }

    void LogicalDevice::destroyBuffer(VkBuffer buffer)
    {
        vkDestroyBuffer(mLogicalDevice, buffer, nullptr);
        mNumBuffers--;
    }

    vk::VkOwner<VkDeviceMemory> LogicalDevice::allocateMemory(VkDeviceSize size, U32 memoryIndex)
    {
        VkDeviceMemory mem = VK_NULL_HANDLE;

        VkMemoryAllocateInfo memAllocInfo = {};
        memAllocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memAllocInfo.pNext = VK_NULL_HANDLE;
        memAllocInfo.allocationSize = size;
        memAllocInfo.memoryTypeIndex = memoryIndex;

        vkAllocateMemory(mLogicalDevice, &memAllocInfo, nullptr, &mem);

        return mem;
    }

    void LogicalDevice::dequeFreeMemory(vk::VkOwner<VkDeviceMemory> memory)
    {
        VkDeviceMemory mem = memory.release();
        vkFreeMemory(mLogicalDevice, mem, nullptr);
    }

} // namespace swizzle