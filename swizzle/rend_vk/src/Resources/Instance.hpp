
#pragma once

/* Include files */

#include <rend_vk/Context.hpp>

#include "Debug.hpp"
#include "Vk.hpp"
#include "_fw.hpp"

#include <memory>
#include <string>
#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace rvk
{
    struct PhysicalDeviceFeatures
    {
        VkPhysicalDeviceFeatures2 mFeatures;
        VkPhysicalDeviceVulkan13Features mMaintenace13;
        VkPhysicalDeviceMeshShaderFeaturesEXT mMeshShaders;
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR mRayTracing;
        VkPhysicalDeviceDynamicRenderingFeatures mDynamicRendering;
    };

    struct FeatureEnable
    {
        VkBool32 mGeometryShaders;
        VkBool32 mPipelinesStatistics;
        VkBool32 mTesselationShader;
        VkBool32 mFillModeNonSolid;

        VkBool32 mMaintenance4;

        VkBool32 mMeshShader;
        VkBool32 mRayTracing;
    };

} // namespace rvk

/* Class Declaration */

namespace rvk
{
    class Instance : public std::enable_shared_from_this<Instance>
    {
    public:
        Instance(bool debugCallbacks);
        virtual ~Instance();

        bool isValid() const;

        uint32_t getNumDevices() const;
        std::string getDeviceName(uint32_t devIdx) const;
        bool isDiscreteGpu(uint32_t devIdx) const;
        bool hasFeatureSupport(uint32_t devIdx, const Features& f) const;

        std::shared_ptr<Device> createDevice(const DeviceCreateInfo& createInfo);

        VkAllocationCallbacks* getAllocCallbacks() const;

        VkInstance getHandle() const;

    private:
        VkPhysicalDevice getDevice(uint32_t index) const;
        std::vector<std::string> listAvailableExtensions(VkPhysicalDevice dev);
        bool addExtensionIfExisting(std::vector<const char*>& extensions,
                                    const std::vector<std::string>& availableExtensions,
                                    const char* extensionName);

        void init();
        void cleanup();

        void createInstance();
        void initDebug();
        void cleanupDebug();

        VkInstance mInstance;

        VkAllocationCallbacks* mAllocCallbacks;

        const bool mUseDebugCallbacks;
        DebugCallbacks mDebugCallbacks;
    };
} // namespace rvk

/* Function Declaration */
