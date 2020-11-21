#include "VulkanPhysicalDevice.hpp"
#include <swizzle/core/logging/Logging.hpp>

#include <array>

namespace swizzle
{

    PhysicalDevice::PhysicalDevice(VkPhysicalDevice phys)
        : mPhysicalDevice(phys)
    {
    }

    std::vector<std::string> PhysicalDevice::listAvailableExtensions()
    {

        std::vector<std::string> extensions;

        uint32_t count = 0;
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

    uint32_t PhysicalDevice::getNumQueues() const
    {
        uint32_t queueCount = 0;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueProperties;
        queueProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueProperties.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++)
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

    LogicalDevice* PhysicalDevice::createLogicalDevice(std::vector<float>& queuPrio)
    {
        return new LogicalDevice(mPhysicalDevice, mActiveExtensions, queuPrio);
    }

    LogicalDevice::LogicalDevice(VkPhysicalDevice phys, const std::vector<const char*>& extensions,
                                 std::vector<float>& queuePrio)
    {
        mPhysicalDevice = phys;
        float* prios = queuePrio.data();

        uint32_t queueFamilyIndex = 0;

        VkDeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.queueCount = static_cast<uint32_t>(queuePrio.size());
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.pQueuePriorities = prios;
        queueCreateInfo.flags = 0;

        VkDeviceCreateInfo createInfo;
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.enabledLayerCount = 0;

        createInfo.flags = 0;
        createInfo.pEnabledFeatures = nullptr;
        createInfo.pNext = nullptr;
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;

        VkResult res = vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("vkCreateDevice failed!, returned %s\n", VkResultToString(res));
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

    VkQueue LogicalDevice::getQueue(uint32_t queueFamily, uint32_t queueIndex) const
    {
        VkQueue queue = VK_NULL_HANDLE;

        Id v;
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

} // namespace swizzle