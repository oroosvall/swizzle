
/* Include files */

#include "VkInstanceMock.hpp"
#include <vulkan/vulkan.h>

#include <vector>

#include <gtest/gtest.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

struct Device
{
    const VkAllocationCallbacks* mAllocCb;
};

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties)
{
    memset(pProperties, 0, sizeof(VkPhysicalDeviceProperties));
    PhysicalDeviceMockInfo* pDev = (PhysicalDeviceMockInfo*)physicalDevice;
    if (pDev)
    {
        memcpy(pProperties->deviceName, pDev->mName.c_str(), pDev->mName.size());
    }
}

void vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice,
                                              uint32_t* pQueueFamilyPropertyCount,
                                              VkQueueFamilyProperties* pQueueFamilyProperties)
{
}

void vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures)
{
    PhysicalDeviceMockInfo* pDev = (PhysicalDeviceMockInfo*)physicalDevice;
    if (pDev)
    {
        VkPhysicalDeviceFeatures2* f = pFeatures;
        while (f)
        {
            void* next = f->pNext;
            switch (f->sType)
            {
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2:
                memcpy(&f->features, &pDev->mPhysicalDeviceFeatures, sizeof(VkPhysicalDeviceFeatures));
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES:
                memcpy(f, &pDev->mMaintenace13Features, sizeof(VkPhysicalDeviceVulkan13Features));
                f->pNext = next;
                f->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT:
                memcpy(f, &pDev->mMeshShaderFeatures, sizeof(VkPhysicalDeviceMeshShaderFeaturesEXT));
                f->pNext = next;
                f->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR:
                memcpy(f, &pDev->mRayTracingFeatures, sizeof(VkPhysicalDeviceRayTracingPipelineFeaturesKHR));
                f->pNext = next;
                f->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
                break;
            default:
                break;
            }
            f = (VkPhysicalDeviceFeatures2*)f->pNext;
        }
    }
}

VkResult vkCreateDevice(VkPhysicalDevice physicalDevice,
                        const VkDeviceCreateInfo* pCreateInfo,
                        const VkAllocationCallbacks* pAllocator,
                        VkDevice* pDevice)
{
    Device* d = nullptr;
    if (pAllocator)
    {
        void* p = pAllocator->pfnAllocation(pAllocator->pUserData,
                                            sizeof(Device),
                                            8,
                                            VkSystemAllocationScope::VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        d = new (p)Device();
    }
    else
    {
        d = new Device();
    }
    d->mAllocCb = pAllocator;
    *pDevice = (VkDevice)d;

    return VK_SUCCESS;
}

void vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
    Device* d = (Device*)device;
    if (d)
    {
        GTEST_ASSERT_EQ(d->mAllocCb, pAllocator);
        if (pAllocator)
        {
            d->~Device();
            pAllocator->pfnFree(pAllocator->pUserData, d);
        }
        else
        {
            delete d;
        }
    }
}

VkResult vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                              const char* pLayerName,
                                              uint32_t* pPropertyCount,
                                              VkExtensionProperties* pProperties)
{
    PhysicalDeviceMockInfo* pDev = (PhysicalDeviceMockInfo*)physicalDevice;
    if (pDev)
    {
        if (pPropertyCount && pProperties == nullptr)
        {
            *pPropertyCount = pDev->mExtensions.size();
        }
        else if (pPropertyCount && *pPropertyCount != 0 && pProperties)
        {
            for (uint32_t i = 0u; i < *pPropertyCount; ++i)
            {
                memcpy(pProperties[i].extensionName, pDev->mExtensions[i], strlen(pDev->mExtensions[i]));
            }
            return VK_SUCCESS;
        }
    }
    return VK_SUCCESS;
}

VkResult vkDeviceWaitIdle(VkDevice device)
{
    return VK_SUCCESS;
}

void VkDeviceMockInitialize()
{
}

void VkDeviceMockCleanup()
{
}