#ifndef INSTANCE_HPP
#define INSTANCE_HPP

/* Include files */

#include "_fwDecl.hpp"

#include <swizzle/gfx/Context.hpp>
#include "../backend/Vk.hpp"

#include <vector>
#include <string>

#include <memory>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class Instance : public std::enable_shared_from_this<Instance>
    {
    public:
        Instance(const swizzle::gfx::GfxContextCreateInfo& createInfo);
        virtual ~Instance();

        U32 getNumDevices();
        SwChar* getDeviceName(U32 index);
        std::vector<std::string> listAvailableExtensions(U32 index);

        common::Resource<Device> initializeDevice(const swizzle::gfx::GfxContextInitializeInfo& createInfo,
                        const std::vector<const char*>& extensions);

        VkAllocationCallbacks* getAllocCallbacks() const;

        VkInstance getInstanceHandle() const;

    private:

        VkPhysicalDevice getDevice(U32 index);

        void initializeInstance();
        void cleanup();

        void createInstance();
        void initDebug();
        void deinitDebug();

        void addLayerIfExisting(std::vector<const char*>& layers, const std::vector<VkLayerProperties>& availableLayers,
                                const char* layerName);

        VkInstance mInstance;

        VkAllocationCallbacks* mAllocCallbacks;

        VkPhysicalDeviceProperties mSelectedDeviceProperties;

        // Debug stuff
        SwBool mEnableDebugCallbacks;
        VkDebugReportCallbackEXT mDebugCallback;

        PFN_vkCreateDebugReportCallbackEXT mCreateDebugCallback;
        PFN_vkDestroyDebugReportCallbackEXT mDestroyDebugCallback;
    };
}

/* Function Declaration */

#endif