#include "VulkanInstance.hpp"
#include "Surface.hpp"
#include <swizzle/core/Logging.hpp>
#include "backend/VkDebug.hpp"

namespace swizzle::gfx
{

    VulkanInstance::VulkanInstance()
        : mVkInstance(VK_NULL_HANDLE)
        , mSurface(VK_NULL_HANDLE)
        , mDebugCallback(VK_NULL_HANDLE)
        , mCreateDebugCallback(nullptr)
        , mDestroyDebugCallback(nullptr)
    {
        createInstance();
#ifdef SW_DEBUG
        initDebug();
#endif
    }

    VulkanInstance::~VulkanInstance()
    {
#ifdef SW_DEBUG
        deinitDebug();
#endif
        vkDestroyInstance(mVkInstance, vk::getDebugAllocCallback());
    }

    const VkInstance VulkanInstance::getInstance() const
    {
        return mVkInstance;
    }

    std::vector<PhysicalDevice> VulkanInstance::listDevices()
    {
        uint32_t count = 0;
        vkEnumeratePhysicalDevices(mVkInstance, &count, VK_NULL_HANDLE);
        std::vector<VkPhysicalDevice> devices;
        devices.resize(count);
        vkEnumeratePhysicalDevices(mVkInstance, &count, devices.data());

        std::vector<PhysicalDevice> devs;

        for (auto it : devices)
        {
            devs.push_back(it);
        }

        return devs;
    }

    void VulkanInstance::createInstance()
    {

        VkDebugReportCallbackCreateInfoEXT debugCallbackInfo = {};
        memset(&debugCallbackInfo, 0, sizeof(VkDebugReportCallbackCreateInfoEXT));
        debugCallbackInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debugCallbackInfo.flags = VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT;
        debugCallbackInfo.pNext = nullptr;
        debugCallbackInfo.pUserData = nullptr;
        debugCallbackInfo.pfnCallback = &vk::vkDebugCallback;

        VkInstanceCreateInfo vkInstanceInfo;
        memset(&vkInstanceInfo, 0, sizeof(VkInstanceCreateInfo));

        std::vector<const char*> extensions;

        extensions.push_back(GetOsSurfaceExtension());
        extensions.push_back("VK_KHR_surface");
        // extensions.push_back("VK_KHR_Maintenance1");
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        //extensions.push_back();

        std::vector<const char*> debugLayers;
        debugLayers.push_back("VK_LAYER_KHRONOS_validation");
        debugLayers.push_back("VK_LAYER_RENDERDOC_Capture");

        VkApplicationInfo vkAppInfo = {};
        vkAppInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkAppInfo.apiVersion = VK_API_VERSION_1_2;
        vkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        vkAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        vkAppInfo.pApplicationName = "App";
        vkAppInfo.pEngineName = "Swizzle Engine";
        vkAppInfo.pNext = nullptr;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.enabledLayerCount = static_cast<uint32_t>(debugLayers.size());
        createInfo.pNext = &debugCallbackInfo;
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.ppEnabledLayerNames = debugLayers.data();
        createInfo.pApplicationInfo = &vkAppInfo;

        VkResult res = vkCreateInstance(&createInfo, vk::getDebugAllocCallback(), &mVkInstance);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("vkCreateInstance failed!, returned %s\n", vk::VkResultToString(res));
        }

    }


    void VulkanInstance::initDebug()
    {

        mCreateDebugCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(mVkInstance, "vkCreateDebugReportCallbackEXT"));
        mDestroyDebugCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(mVkInstance, "vkDestroyDebugReportCallbackEXT"));

        VkDebugReportCallbackCreateInfoEXT debugCallbackInfo;
        memset(&debugCallbackInfo, 0, sizeof(VkDebugReportCallbackCreateInfoEXT));
        debugCallbackInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debugCallbackInfo.flags = VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
            //VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT;
        debugCallbackInfo.pNext = nullptr;
        debugCallbackInfo.pUserData = nullptr;
        debugCallbackInfo.pfnCallback = &vk::vkDebugCallback;

        if (mCreateDebugCallback)
        {
            mCreateDebugCallback(mVkInstance, &debugCallbackInfo, vk::getDebugAllocCallback(), &mDebugCallback);
        }
    }

    void VulkanInstance::deinitDebug()
    {
        if (mDestroyDebugCallback)
        {
            mDestroyDebugCallback(mVkInstance, mDebugCallback, vk::getDebugAllocCallback());
        }
    }

} // namespace swizzle