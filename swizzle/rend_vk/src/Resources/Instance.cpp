
/* Include files */

#include "Instance.hpp"
#include "Debug.hpp"
#include "Extension.hpp"
#include "Surface.hpp"

#include "../Log.hpp"

#include <algorithm>
#include <numeric>
#include <optional>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

namespace rvk
{
    static std::vector<const char*> getExtensions();
    static std::vector<VkLayerProperties> getLayers();

    static std::vector<const char*> getInstanceLayers(const std::vector<VkLayerProperties>& layers);

#if _DEBUG
    static void addLayerIfExists(std::vector<const char*>& layers,
                                 const std::vector<VkLayerProperties>& available,
                                 const char* layerName);
#endif

    static void setupVkFeatureChain(PhysicalDeviceFeatures& features);
    static PhysicalDeviceFeatures getFeatures(VkPhysicalDevice dev);

    static std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice dev);

    static uint32_t getQueueFamilyIndex(const std::vector<VkQueueFamilyProperties> queueFamilies,
                                        VkQueueFlags mask,
                                        VkQueueFlags flags);

    static bool setFeature(VkBool32* out, const VkBool32* const in, FeatureState fs);
    static bool enableFeatures(PhysicalDeviceFeatures& ef, const PhysicalDeviceFeatures& af, const Features& f);

    static std::optional<VkDeviceQueueCreateInfo> setQueueInfo(std::vector<float>& prios, uint32_t queueFamilyIndex);

} // namespace rvk

/* Static Function Definition */

namespace rvk
{
    static std::vector<const char*> getExtensions()
    {
        std::vector<const char*> extensions;

        extensions.push_back(GetOsSurfaceExtension());
        extensions.push_back(GetKHRSurfaceExtension());

#if _DEBUG
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

        return extensions;
    }

    static std::vector<VkLayerProperties> getLayers()
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> layers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        return layers;
    }

    static std::vector<const char*> getInstanceLayers(const std::vector<VkLayerProperties>& layers)
    {
        RVK_UNUSED_ARG(layers);
        std::vector<const char*> instanceLayers;
#if _DEBUG
        // extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        addLayerIfExists(instanceLayers, layers, "VK_LAYER_KHRONOS_validation");
        addLayerIfExists(instanceLayers, layers, "VK_LAYER_RENDERDOC_Capture");
#endif
        return instanceLayers;
    }

#if _DEBUG
    static void addLayerIfExists(std::vector<const char*>& layers,
                                 const std::vector<VkLayerProperties>& available,
                                 const char* layerName)
    {
        for (const auto& lyr : available)
        {
            if (strcmp(lyr.layerName, layerName) == 0)
            {
                layers.emplace_back(layerName);
            }
        }
    }
#endif

    static void setupVkFeatureChain(PhysicalDeviceFeatures& features)
    {
        features.mFeatures.sType     = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.mMaintenace13.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features.mMeshShaders.sType  = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
        features.mRayTracing.sType =
            VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;

        features.mFeatures.pNext     = &features.mMaintenace13;
        features.mMaintenace13.pNext = &features.mMeshShaders;
        features.mMeshShaders.pNext  = &features.mRayTracing;

        features.mRayTracing.pNext = VK_NULL_HANDLE;
    }

    static PhysicalDeviceFeatures getFeatures(VkPhysicalDevice dev)
    {
        PhysicalDeviceFeatures features{};
        setupVkFeatureChain(features);

        vkGetPhysicalDeviceFeatures2(dev, &features.mFeatures);

        return features;
    }

    static bool setFeature(VkBool32* out, const VkBool32* const in, FeatureState fs)
    {
        if (fs != FeatureState::No)
            *out = *in;

        if ((fs == FeatureState::Required && *out) || fs == FeatureState::No)
            return true;

        return false;
    }

    static bool enableFeatures(PhysicalDeviceFeatures& ef, const PhysicalDeviceFeatures& af, const Features& f)
    {
        bool ok = true;

        ok &= setFeature(&ef.mFeatures.features.geometryShader,
                         &af.mFeatures.features.geometryShader,
                         f.mGeometryShaders);

        ok &= setFeature(&ef.mFeatures.features.pipelineStatisticsQuery,
                         &af.mFeatures.features.pipelineStatisticsQuery,
                         FeatureState::Required);

        ok &= setFeature(&ef.mFeatures.features.tessellationShader,
                         &af.mFeatures.features.tessellationShader,
                         f.mTesselation);

        ok &= setFeature(&ef.mFeatures.features.fillModeNonSolid,
                         &af.mFeatures.features.fillModeNonSolid,
                         FeatureState::Required);

        ok &= setFeature(&ef.mMaintenace13.maintenance4, &af.mMaintenace13.maintenance4, FeatureState::Required);

        ok &= setFeature(&ef.mMeshShaders.meshShader, &af.mMeshShaders.meshShader, f.mMeshShader);
        ok &= setFeature(&ef.mMeshShaders.taskShader, &af.mMeshShaders.taskShader, f.mMeshShader);

        ok &= setFeature(&ef.mRayTracing.rayTracingPipeline, &af.mRayTracing.rayTracingPipeline, f.mRayTracing);

        return ok;
    }

    static std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice dev)
    {
        uint32_t count = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
        std::vector<VkQueueFamilyProperties> props(count);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, props.data());

        return props;
    }

    static uint32_t getQueueFamilyIndex(const std::vector<VkQueueFamilyProperties> queueFamilies,
                                        VkQueueFlags mask,
                                        VkQueueFlags flags)
    {
        uint32_t idx = 0u;
        for (auto& it : queueFamilies)
        {
            if ((it.queueFlags & mask) == flags)
            {
                break;
            }
            idx++;
        }

        return idx;
    }

    static std::optional<VkDeviceQueueCreateInfo> setQueueInfo(std::vector<float>& prios, uint32_t queueFamilyIndex)
    {
        if (prios.empty())
        {
            return std::nullopt;
        }

        float sum = std::reduce(prios.begin(), prios.end());
        std::transform(prios.cbegin(), prios.cend(), prios.begin(), [sum](float f) { return f / sum; });

        VkDeviceQueueCreateInfo info{};
        info.sType            = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.pNext            = nullptr;
        info.queueCount       = static_cast<U32>(prios.size());
        info.queueFamilyIndex = queueFamilyIndex;
        info.pQueuePriorities = prios.data();
        info.flags            = 0u;

        return info;
    }

} // namespace rvk

/* Function Definition */

/* Class Public Function Definition */

namespace rvk
{
    Instance::Instance(bool debugCallbacks)
        : mInstance(VK_NULL_HANDLE)
        , mAllocCallbacks(nullptr)
        , mUseDebugCallbacks(debugCallbacks)
        , mDebugCallbacks()
    {
        init();
    }

    Instance::~Instance()
    {
        cleanup();
    }

    bool Instance::isValid() const
    {
        return mInstance != VK_NULL_HANDLE;
    }

    uint32_t Instance::getNumDevices() const
    {
        U32 deviceCount = 0u;
        VkResult res    = vkEnumeratePhysicalDevices(mInstance, &deviceCount, VK_NULL_HANDLE);
        LogVulkanError(res, "vkEnumeratePhysicalDevices");

        return deviceCount;
    }

    std::string Instance::getDeviceName(uint32_t devIdx) const
    {
        const char* name     = "[No GPU]";
        VkPhysicalDevice dev = getDevice(devIdx);

        if (dev)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);
            name = props.deviceName;
        }

        return name;
    }

    bool Instance::isDiscreteGpu(uint32_t devIdx) const
    {
        bool dedicated       = false;
        VkPhysicalDevice dev = getDevice(devIdx);
        if (dev)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);
            dedicated = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        }

        return dedicated;
    }

    bool Instance::hasFeatureSupport(uint32_t devIdx, const Features& f) const
    {
        bool features        = true;
        VkPhysicalDevice dev = getDevice(devIdx);
        if (dev)
        {
            PhysicalDeviceFeatures feats = getFeatures(dev);
            if (f.mMeshShader == FeatureState::Required && !feats.mMeshShaders.meshShader &&
                !feats.mMeshShaders.taskShader)
            {
                features = false;
            }
            if (f.mRayTracing == FeatureState::Required && !feats.mRayTracing.rayTracingPipeline)
            {
                features = false;
            }
            if (f.mGeometryShaders == FeatureState::Required && !feats.mFeatures.features.geometryShader)
            {
                features = false;
            }
            if (f.mTesselation == FeatureState::Required && !feats.mFeatures.features.tessellationShader)
            {
                features = false;
            }
        }
        else
        {
            features = false;
        }

        return features;
    }

    std::shared_ptr<int> Instance::createDevice(const DeviceCreateInfo& createInfo)
    {
        std::shared_ptr<int> outDevice = nullptr;
        VkPhysicalDevice phys          = getDevice(createInfo.mDeviceIndex);
        if (phys)
        {
            const std::vector<std::string> extensions = listAvailableExtensions(phys);
            std::vector<const char*> enabledExtensions;

            addExtensionIfExisting(enabledExtensions, extensions, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
            addExtensionIfExisting(enabledExtensions, extensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            addExtensionIfExisting(enabledExtensions, extensions, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME);

            const PhysicalDeviceFeatures availableFeatures = getFeatures(phys);
            PhysicalDeviceFeatures enabledFeatures{};
            setupVkFeatureChain(enabledFeatures);

            bool enabledOk = enableFeatures(enabledFeatures, availableFeatures, createInfo.mDesiredFeatures);
            if (!enabledOk)
            {
                LogMessage(LogLevel::Error, "Device missing required features");
                return nullptr;
            }

            addExtensionIfExisting(enabledExtensions, extensions, VK_KHR_MAINTENANCE_4_EXTENSION_NAME);

            // Mesh shader feature required but not available as extension
            if (enabledFeatures.mMeshShaders.meshShader && enabledFeatures.mMeshShaders.taskShader)
            {
                if (!addExtensionIfExisting(enabledExtensions, extensions, VK_EXT_MESH_SHADER_EXTENSION_NAME))
                {
                    LogMessage(LogLevel::Error, "Mesh shading extension required but not found");
                    return nullptr;
                }
            }
            if (enabledFeatures.mRayTracing.rayTracingPipeline)
            {
                if (!addExtensionIfExisting(enabledExtensions, extensions, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
                {
                    LogMessage(LogLevel::Error, "Ray tracing extension required but not found");
                    return nullptr;
                }
            }

            std::vector<VkQueueFamilyProperties> queues = getQueueFamilyProperties(phys);

            uint32_t queueInfoCount = 0u;
            std::array<VkDeviceQueueCreateInfo, 3> queueInfos{};

            std::vector<F32> genPrio  = createInfo.mGenericQueues;
            std::vector<F32> tfxPrio  = createInfo.mTransferOnlyQueues;
            std::vector<F32> compPrio = createInfo.mAsyncCompute;

            // clang-format off
            U32 genericIdx = getQueueFamilyIndex(queues,
                                                 VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT,
                                                 VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT
            );

            U32 computeIdx = getQueueFamilyIndex(queues,
                                                 VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT |
                                                 VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT,
                                                 VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT
            );
            U32 transferIdx = getQueueFamilyIndex(queues,
                                                  VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT |
                                                  VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT |
                                                  VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT,
                                                  VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT
            );
            // clang-format on

            bool requiredQueueFound = false;

            auto qi = setQueueInfo(genPrio, genericIdx);
            if (qi.has_value())
            {
                queueInfos[queueInfoCount] = qi.value();
                queueInfoCount++;
                requiredQueueFound = true;
            }

            qi = setQueueInfo(compPrio, computeIdx);
            if (qi.has_value())
            {
                queueInfos[queueInfoCount] = qi.value();
                queueInfoCount++;
                requiredQueueFound = true;
            }

            qi = setQueueInfo(tfxPrio, transferIdx);
            if (qi.has_value())
            {
                queueInfos[queueInfoCount] = qi.value();
                queueInfoCount++;
            }

            if (!requiredQueueFound)
            {
                LogMessage(LogLevel::Error, "No Compute or Graphics queues allocated");
                return nullptr;
            }

            VkDeviceCreateInfo info{};
            info.sType                   = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            info.enabledExtensionCount   = static_cast<U32>(enabledExtensions.size());
            info.enabledLayerCount       = 0u;
            info.flags                   = 0u;
            info.pEnabledFeatures        = VK_NULL_HANDLE;
            info.pNext                   = &enabledFeatures.mFeatures;
            info.ppEnabledExtensionNames = enabledExtensions.data();
            info.ppEnabledLayerNames     = VK_NULL_HANDLE;
            info.pQueueCreateInfos       = queueInfos.data();
            info.queueCreateInfoCount    = queueInfoCount;

            VkDevice dev = VK_NULL_HANDLE;
            VkResult res = vkCreateDevice(phys, &info, mAllocCallbacks, &dev);
            LogVulkanError(res, "initialize::vkCreateDevice");

            std::shared_ptr<Extensions> exts = std::make_shared<Extensions>(shared_from_this(), enabledFeatures);

            if (dev)
            {
                outDevice = std::make_shared<int>(1);
                /*outDevice = common::CreateRef<Device>(shared_from_this(), phys, dev);
                outDevice->initAfterConstruction();*/
            }
        }

        return outDevice;
    }

    VkAllocationCallbacks* Instance::getAllocCallbacks() const
    {
        return mAllocCallbacks;
    }

    VkInstance Instance::getHandle() const
    {
        return mInstance;
    }

} // namespace rvk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace rvk
{
    VkPhysicalDevice Instance::getDevice(uint32_t index) const
    {
        VkPhysicalDevice dev = VK_NULL_HANDLE;

        U32 deviceCount = 0u;
        VkResult res    = vkEnumeratePhysicalDevices(mInstance, &deviceCount, VK_NULL_HANDLE);
        LogVulkanError(res, "getDevice::vkEnumeratePhysicalDevices");
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
                LogMessage(LogLevel::Warning, "GPU index out of range!");
            }
        }
        else
        {
            LogMessage(LogLevel::Error, "Failed to find Vulkan compatible GPU!");
        }

        return dev;
    }

    std::vector<std::string> Instance::listAvailableExtensions(VkPhysicalDevice dev)
    {
        std::vector<std::string> extensions;

        U32 count = 0u;
        vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> exts;
        exts.resize(count);
        vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, exts.data());

        for (const auto& it : exts)
        {
            extensions.push_back(it.extensionName);
        }

        return extensions;
    }

    bool Instance::addExtensionIfExisting(std::vector<const char*>& extensions,
                                          const std::vector<std::string>& availableExtensions,
                                          const char* extensionName)
    {
        for (const auto& ext : availableExtensions)
        {
            if (ext == extensionName)
            {
                extensions.emplace_back(extensionName);
                return true;
            }
        }
        return false;
    }

    void Instance::init()
    {
        createInstance();
        if (mUseDebugCallbacks)
        {
            initDebug();
        }
    }

    void Instance::cleanup()
    {
        if (mUseDebugCallbacks)
        {
            cleanupDebug();
        }

        vkDestroyInstance(mInstance, mAllocCallbacks);
        mInstance = VK_NULL_HANDLE;
    }

    void Instance::createInstance()
    {
        std::vector<const char*> extensions            = getExtensions();
        std::vector<VkLayerProperties> availableLayers = getLayers();
        std::vector<const char*> enabledLayers         = getInstanceLayers(availableLayers);

        VkApplicationInfo vkAppInfo  = {};
        vkAppInfo.sType              = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkAppInfo.apiVersion         = VK_API_VERSION_1_3;
        vkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        vkAppInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        vkAppInfo.pApplicationName   = "App";
        vkAppInfo.pEngineName        = "Swizzle Engine";
        vkAppInfo.pNext              = VK_NULL_HANDLE;

        VkDebugReportCallbackCreateInfoEXT debugCallbackInfo = getDebugCallbacks();

        VkInstanceCreateInfo createInfo  = {};
        createInfo.sType                 = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.enabledLayerCount     = static_cast<uint32_t>(enabledLayers.size());
        createInfo.pNext                 = VK_NULL_HANDLE;
        if (mUseDebugCallbacks)
        {
            createInfo.pNext = &debugCallbackInfo;
        }
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.ppEnabledLayerNames     = enabledLayers.data();
        createInfo.pApplicationInfo        = &vkAppInfo;

        VkResult res = vkCreateInstance(&createInfo, mAllocCallbacks, &mInstance);
        if (res != VK_SUCCESS)
        {
            LogMessage(LogLevel::Error, "vkCreateInstance failed!, returned %s\n", VkResultToString(res));
        }
    }

    void Instance::initDebug()
    {
        mDebugCallbacks.mCreate = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT"));
        mDebugCallbacks.mDestroy = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT"));

        if (mDebugCallbacks.mCreate && mDebugCallbacks.mDestroy)
        {
            VkDebugReportCallbackCreateInfoEXT debugCallbackInfo = getDebugCallbacks();

            VkResult res = mDebugCallbacks.mCreate(mInstance,
                                                   &debugCallbackInfo,
                                                   mAllocCallbacks,
                                                   &mDebugCallbacks.mDebugCallback);
            LogVulkanError(res, "mCreateDebugCallback");
        }
    }

    void Instance::cleanupDebug()
    {
        if (mDebugCallbacks.mCreate && mDebugCallbacks.mDestroy)
        {
            mDebugCallbacks.mDestroy(mInstance, mDebugCallbacks.mDebugCallback, mAllocCallbacks);
        }
        memset(&mDebugCallbacks, 0, sizeof(mDebugCallbacks));
    }

} // namespace rvk
