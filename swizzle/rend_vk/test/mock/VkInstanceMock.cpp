
/* Include files */

#include <vulkan/vulkan.h>
#include "VkInstanceMock.hpp"

#include <math.h>
#include <vector>
#include <memory>

#include <gtest/gtest.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

struct Instance
{
    size_t mId;
    const VkAllocationCallbacks* mAllocCb;
    uint32_t numDevices;
};

/* Static Function Declaration */

static bool testExtensions(uint32_t enabledExtensionCount, const char* const* ppEnabledExtensionNames);

static VkResult createDebugReportCallback(VkInstance instance,
                                          const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugReportCallbackEXT* pCallback);

static void destroyDebugReportCallback(VkInstance instance,
                                       VkDebugReportCallbackEXT callback,
                                       const VkAllocationCallbacks* pAllocator);

/* Static Variables */

InstanceMockInit gMockInit        = InstanceMockInit::InstanceInvalid;
VkResult gCreateInstanceResult    = VkResult::VK_ERROR_INITIALIZATION_FAILED;
std::vector<Instance*> gInstances = {};
std::vector<PhysicalDeviceMockInfo> gPhysicalDeviceMockInfos = {};

std::vector<const char*> gAvailableExtensions;
uint32_t gDebugCallbacksInit = 0;

/* Static Function Definition */

static bool testExtensions(uint32_t enabledExtensionCount, const char* const* ppEnabledExtensionNames)
{
    bool allFound = true;
    for (size_t i = 0; i < enabledExtensionCount; i++)
    {
        bool f = false;
        for (size_t j = 0; j < gAvailableExtensions.size(); j++)
        {
            auto len = std::min(strlen(ppEnabledExtensionNames[i]), strlen(gAvailableExtensions[j]));
            if (strncmp(ppEnabledExtensionNames[i], gAvailableExtensions[j], len) == 0)
            {
                f = true;
                break;
            }
        }
        allFound &= f;
    }
    return allFound;
}

static VkResult createDebugReportCallback(VkInstance instance,
                                          const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugReportCallbackEXT* pCallback)
{
    (void)instance;
    (void)pCreateInfo;
    (void)pAllocator;
    (void)pCallback;

    gDebugCallbacksInit++;
    return VkResult::VK_SUCCESS;
}

static void destroyDebugReportCallback(VkInstance instance,
                                       VkDebugReportCallbackEXT callback,
                                       const VkAllocationCallbacks* pAllocator)
{
    (void)instance;
    (void)callback;
    (void)pAllocator;

    gDebugCallbacksInit--;
}

/* Function Definition */

PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance inst, const char* fn)
{
    PFN_vkVoidFunction func = nullptr;

    if (fn == std::string("vkCreateDebugReportCallbackEXT"))
    {
        func = (PFN_vkVoidFunction)createDebugReportCallback;
    }
    else if (fn == std::string("vkCreateDebugReportCallbackEXT"))
    {
        func = (PFN_vkVoidFunction)destroyDebugReportCallback;
    }

    return func;
}

VkResult vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                          const VkAllocationCallbacks* pAllocator,
                          VkInstance* pInstance)
{
    switch (gMockInit)
    {
    case InstanceMockInit::InstanceInvalid:
        gCreateInstanceResult = VkResult::VK_ERROR_INITIALIZATION_FAILED;
        break;
    case InstanceMockInit::InstanceInvalid_NoExtensions:
    case InstanceMockInit::InstanceValid: {
        gCreateInstanceResult = VkResult::VK_SUCCESS;
        Instance* i           = nullptr;

        if (!testExtensions(pCreateInfo->enabledExtensionCount, pCreateInfo->ppEnabledExtensionNames))
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        if (pAllocator)
        {
            void* p = pAllocator->pfnAllocation(pAllocator->pUserData,
                                      sizeof(Instance),
                                      8,
                                      VkSystemAllocationScope::VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            i = new (p)Instance();
        }
        else
        {
            i = new Instance();
        }
        i->mId        = gInstances.size();
        i->mAllocCb   = pAllocator;
        i->numDevices = 1u;
        gInstances.push_back(i);
        *pInstance = (VkInstance)i;
        break;
    }
    default:
        break;
    }

    return gCreateInstanceResult;
}

void vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    Instance* i = (Instance*)instance;
    if (i)
    {
        size_t idx = i->mId;
        GTEST_ASSERT_EQ(gInstances[idx], i);
        GTEST_ASSERT_EQ(i->mAllocCb, pAllocator);
        if (pAllocator)
        {
            i->~Instance();
            pAllocator->pfnFree(pAllocator->pUserData, i);
        }
        else
        {
            delete i;
        }
        gInstances[idx] = nullptr;
    }
}

VkResult vkEnumeratePhysicalDevices(VkInstance instance,
                                    uint32_t* pPhysicalDeviceCount,
                                    VkPhysicalDevice* pPhysicalDevices)
{
    Instance* inst = (Instance*)instance;
    if (inst)
    {
        if (pPhysicalDeviceCount && pPhysicalDevices == nullptr)
        {
            *pPhysicalDeviceCount = inst->numDevices;
            return VK_SUCCESS;
        }
        else if (pPhysicalDeviceCount && *pPhysicalDeviceCount != 0 && pPhysicalDevices)
        {
            for (uint32_t i = 0u; i < *pPhysicalDeviceCount; ++i)
            {
                pPhysicalDevices[i] = (VkPhysicalDevice) &gPhysicalDeviceMockInfos[i];
            }
            return VK_SUCCESS;
        }
    }
    else
    {
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

VkResult vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
    return gCreateInstanceResult;
}

void VkInstanceMockInitialize(InstanceMockInit init, std::vector<PhysicalDeviceMockInfo> infos)
{
    switch (init)
    {
    case InstanceMockInit::InstanceValid:
        gAvailableExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        gAvailableExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        gAvailableExtensions.push_back("VK_KHR_win32_surface");
        break;
    case InstanceMockInit::InstanceInvalid_NoExtensions:
    case InstanceMockInit::InstanceInvalid:
    default:
        break;
    }

    gPhysicalDeviceMockInfos = infos;

    gMockInit = init;
}

void VkInstanceMockCleanup()
{
    gCreateInstanceResult = VkResult::VK_ERROR_INITIALIZATION_FAILED;
    gMockInit             = InstanceMockInit::InstanceInvalid;

    for (auto& i : gInstances)
    {
        if (i)
        {
            if (i->mAllocCb)
            {
                auto cb = i->mAllocCb;
                i->~Instance();
                cb->pfnFree(cb->pUserData, i);
            }
            else
            {
                delete i;
            }
        }
    }

    gPhysicalDeviceMockInfos.clear();

    gInstances.clear();
    gAvailableExtensions.clear();

    GTEST_ASSERT_EQ(gDebugCallbacksInit, 0u);
}
