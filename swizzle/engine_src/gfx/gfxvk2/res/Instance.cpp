
/* Include files */

#include <swizzle/core/Logging.hpp>
#include <swizzle/core/Platform.hpp>

#include "Instance.hpp"
#include "../backend/VkDebug.hpp"
#include "../surface/Surface.hpp"

#include "Device.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace vk
{
    Instance::Instance(const swizzle::gfx::GfxContextCreateInfo& createInfo)
        : mInstance(VK_NULL_HANDLE)
        , mAllocCallbacks(GetDebugAllocCallback(createInfo.mDebugMemory))
        , mSelectedDeviceProperties()
        , mEnableDebugCallbacks(createInfo.mDebugCallbacks)
        , mDebugCallback(VK_NULL_HANDLE)
        , mCreateDebugCallback(VK_NULL_HANDLE)
        , mDestroyDebugCallback(VK_NULL_HANDLE)
    {
        initializeInstance();
    }

    Instance::~Instance()
    {
        cleanup();
    }

    U32 Instance::getNumDevices()
    {
        U32 deviceCount = 0u;
        VkResult res = vkEnumeratePhysicalDevices(mInstance, &deviceCount, VK_NULL_HANDLE);
        LogVulkanError(res, "vkEnumeratePhysicalDevices");

        return deviceCount;
    }

    SwChar* Instance::getDeviceName(U32 index)
    {
        const SwChar* name = "[No GPU]";
        VkPhysicalDevice dev = getDevice(index);

        if (dev)
        {
            vkGetPhysicalDeviceProperties(dev, &mSelectedDeviceProperties);
            name = mSelectedDeviceProperties.deviceName;
        }

        return (SwChar*)name;
    }

    SwBool Instance::isDiscreteGpu(U32 index)
    {
        SwBool dedicated = false;
        VkPhysicalDevice dev = getDevice(index);
        if (dev) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(dev, &deviceProperties);
            dedicated = deviceProperties.deviceType ==  VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        }
        return dedicated;
    }

    std::vector<std::string> Instance::listAvailableExtensions(U32 index)
    {
        std::vector<std::string> extensions;

        VkPhysicalDevice dev = getDevice(index);

        if (dev)
        {
            U32 count = 0u;
            vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, nullptr);
            std::vector<VkExtensionProperties> exts;
            exts.resize(count);
            vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, exts.data());

            for (const auto& it : exts)
            {
                extensions.push_back(it.extensionName);
            }
        }

        return extensions;
    }

    common::Resource<Device> Instance::initializeDevice(const swizzle::gfx::GfxContextInitializeInfo& createInfo,
                                                  const std::vector<const char*>& extensions)
    {
        common::Resource<Device> outDevice = nullptr;

        std::vector<F32> queuePrio = std::vector<F32>{1.0F}; // TODO: create prio list
        F32* prios = queuePrio.data();
        U32 queueFamilyIndex = 0u;

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.queueCount = static_cast<U32>(queuePrio.size());
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.pQueuePriorities = prios;
        queueCreateInfo.flags = 0u;

        VkDeviceCreateInfo info{};
        info.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.enabledExtensionCount = static_cast<U32>(extensions.size());
        info.enabledLayerCount = 0u;

        VkPhysicalDevice phys = getDevice(createInfo.mDeviceIndex);

        getDeviceFeatures(phys);

        VkPhysicalDeviceFeatures2 features{};

        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.features.geometryShader = true;
        features.features.pipelineStatisticsQuery = true;
        features.features.tessellationShader = true;
        features.features.fillModeNonSolid = true;

        VkPhysicalDeviceVulkan13Features maint{};
        maint.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        maint.maintenance4 = true;

        features.pNext = &maint;

        VkPhysicalDeviceMeshShaderFeaturesEXT meshFeatures{};
        meshFeatures.meshShader = false;
        meshFeatures.taskShader = false;
        meshFeatures.pNext = VK_NULL_HANDLE;
        meshFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

        maint.pNext = &meshFeatures;

        info.flags = 0u;
        info.pEnabledFeatures = VK_NULL_HANDLE;
        info.pNext = &features;
        info.ppEnabledExtensionNames = extensions.data();
        info.ppEnabledLayerNames = VK_NULL_HANDLE;
        info.pQueueCreateInfos = &queueCreateInfo;
        info.queueCreateInfoCount = 1u;

        VkDevice dev = VK_NULL_HANDLE;

        if (phys)
        {
            VkResult res = vkCreateDevice(phys, &info, mAllocCallbacks, &dev);
            LogVulkanError(res, "initialize::vkCreateDevice");
            if (dev)
            {
                outDevice = common::CreateRef<Device>(shared_from_this(), phys, dev);
                outDevice->initAfterConstruction();
            }
        }

        return outDevice;
    }

    VkAllocationCallbacks* Instance::getAllocCallbacks() const
    {
        return mAllocCallbacks;
    }

    VkInstance Instance::getInstanceHandle() const
    {
        return mInstance;
    }

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{

    VkPhysicalDevice Instance::getDevice(U32 index)
    {
        VkPhysicalDevice dev = VK_NULL_HANDLE;

        U32 deviceCount = 0u;
        VkResult res = vkEnumeratePhysicalDevices(mInstance, &deviceCount, VK_NULL_HANDLE);
        LogVulkanError(res, "getDevice::vkEnumeratePhysicalDevices");
        if (deviceCount != 0u)
        {
            std::vector<VkPhysicalDevice> devices;
            devices.resize(deviceCount);
            res = vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());
            LogVulkanError(res, "getDevice::vkEnumeratePhysicalDevices");

            if (deviceCount > index)
            {
                dev = devices[index];
            }
            else
            {
                LOG_WARNING("GPU index out of range\n");
            }
        }
        else
        {
            LOG_ERROR("Failed to find Vulkan compatible GPU!");
        }

        return dev;
    }

    void Instance::initializeInstance()
    {
        createInstance();
#ifdef SW_DEBUG
        if (mEnableDebugCallbacks)
        {
            initDebug();
        }
#endif
        if (!vk::loadMeshShaderExtensions(mInstance))
        {
            LOG_WARNING("Mesh shader extensions not loaded");
        }
    }

    void Instance::cleanup()
    {
#ifdef SW_DEBUG
        if (mEnableDebugCallbacks)
        {
            deinitDebug();
            mCreateDebugCallback = nullptr;
            mDestroyDebugCallback = nullptr;
        }
#endif
        vkDestroyInstance(mInstance, mAllocCallbacks);

        mInstance = VK_NULL_HANDLE;
    }

    void Instance::createInstance()
    {
        VkDebugReportCallbackCreateInfoEXT debugCallbackInfo{};
        debugCallbackInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debugCallbackInfo.flags = VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT |
                                  VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                  //VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                                  VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                                  VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT;
        debugCallbackInfo.pNext = VK_NULL_HANDLE;
        debugCallbackInfo.pUserData = VK_NULL_HANDLE;
        debugCallbackInfo.pfnCallback = &vkDebugCallback;

        std::vector<const char*> extensions;

        extensions.push_back(GetOsSurfaceExtension());
        extensions.push_back(GetKHRSurfaceExtension());

        uint32_t extensionCount = 0u;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensionList(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionList.data());

        LOG_INFO("Available extensions");

        for(auto& prop: extensionList)
        {
            LOG_INFO("%s ", prop.extensionName);
        }

        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> layers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        std::vector<const char*> instanceLayers;

#if _DEBUG
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        //extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        addLayerIfExisting(instanceLayers, layers, "VK_LAYER_KHRONOS_validation");
        addLayerIfExisting(instanceLayers, layers, "VK_LAYER_RENDERDOC_Capture");
#endif

        //instanceLayers.push_back("VK_LAYER_LUNARG_gfxreconstruct");

        VkApplicationInfo vkAppInfo = {};
        vkAppInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkAppInfo.apiVersion = VK_API_VERSION_1_3;
        vkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        vkAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        vkAppInfo.pApplicationName = "App";
        vkAppInfo.pEngineName = "Swizzle Engine";
        vkAppInfo.pNext = VK_NULL_HANDLE;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
        createInfo.pNext = VK_NULL_HANDLE;
        if (mEnableDebugCallbacks)
        {
            createInfo.pNext = &debugCallbackInfo;
        }
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.ppEnabledLayerNames = instanceLayers.data();
        createInfo.pApplicationInfo = &vkAppInfo;

        VkResult res = vkCreateInstance(&createInfo, mAllocCallbacks, &mInstance);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("vkCreateInstance failed!, returned %s\n", VkResultToString(res));
        }
    }

    void Instance::initDebug()
    {
        mCreateDebugCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT"));
        mDestroyDebugCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT"));

        VkDebugReportCallbackCreateInfoEXT debugCallbackInfo{};
        debugCallbackInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debugCallbackInfo.flags = VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT |
                                  VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                  //VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                                  VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                                  VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT;
        debugCallbackInfo.pNext = VK_NULL_HANDLE;
        debugCallbackInfo.pUserData = VK_NULL_HANDLE;
        debugCallbackInfo.pfnCallback = &vkDebugCallback;

        if (mCreateDebugCallback)
        {
            VkResult res = mCreateDebugCallback(mInstance, &debugCallbackInfo, mAllocCallbacks, &mDebugCallback);
            vk::LogVulkanError(res, "mCreateDebugCallback");
        }
    }

    void Instance::deinitDebug()
    {
        if (mDestroyDebugCallback)
        {
            mDestroyDebugCallback(mInstance, mDebugCallback, mAllocCallbacks);
        }
    }

    void Instance::addLayerIfExisting(std::vector<const char*>& layers,
                                      const std::vector<VkLayerProperties>& availableLayers, const char* layerName)
    {
        for (const auto& lyr : availableLayers)
        {
            if (strcmp(lyr.layerName, layerName) == 0)
            {
                layers.emplace_back(layerName);
            }
        }
    }

    void Instance::getDeviceFeatures(VkPhysicalDevice phys)
    {

        VkPhysicalDeviceFeatures2 features{};

        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

        VkPhysicalDeviceVulkan13Features maint{};
        maint.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

        features.pNext = &maint;

        VkPhysicalDeviceMeshShaderFeaturesEXT meshFeatures{};
        meshFeatures.pNext = VK_NULL_HANDLE;
        meshFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

        maint.pNext = &meshFeatures;

        vkGetPhysicalDeviceFeatures2(phys, &features);

    }

} // namespace vk