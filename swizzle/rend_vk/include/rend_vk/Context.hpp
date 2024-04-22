
#pragma once

/* Include files */

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <rend_vk/GfxDevice.hpp>

/* Defines */

/* Typedefs/enums */

namespace rvk
{
    enum class FeatureState
    {
        No,
        Optional,
        Required,
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace rvk
{
    struct Features
    {
        FeatureState mGeometryShaders;
        FeatureState mTesselation;
        FeatureState mMeshShader;
        FeatureState mRayTracing;
    };

    struct DeviceCreateInfo
    {
        uint32_t mDeviceIndex;

        Features mDesiredFeatures;

        std::vector<float> mGenericQueues;
        std::vector<float> mTransferOnlyQueues;
        std::vector<float> mAsyncCompute;
    };
} // namespace rvk

/* Class Declaration */

namespace rvk
{
    class Context
    {
    public:
        virtual ~Context()
        {
        }

        /// <summary>
        /// Get number of gpu devices in system
        /// </summary>
        /// <returns>count of devices</returns>
        virtual uint32_t getDeviceCount() const = 0;

        /// <summary>
        /// Retrieve the device name
        /// </summary>
        /// <param name="deviceIndex">Device index in range 0 - getDeviceCount()</param>
        /// <returns>Device name or "[No GPU]" on invalid index</returns>
        virtual std::string getDeviceName(uint32_t deviceIndex) const = 0;

        /// <summary>
        /// Get if a device is dedicated gpu device
        /// </summary>
        /// <param name="deviceIndex">Device index in range 0 - getDeviceCount()</param>
        /// <returns>dedicated or false on invalid index</returns>
        virtual bool isDiscreteGpu(uint32_t deviceIndex) const = 0;

        /// <summary>
        /// Get device feature support
        /// </summary>
        /// <param name="devIdx">Device index in range 0 - getDeviceCount()</param>
        /// <param name="features">State with features that are required/optional</param>
        /// <returns>false when required features are not available</returns>
        virtual bool hasFeatureSupport(uint32_t devIdx, const Features& features) const = 0;

        /// <summary>
        /// Create a device from spec
        /// </summary>
        /// <param name="createInfo">spec to use when creating device</param>
        /// <returns>valid device or nullptr on failure</returns>
        virtual std::shared_ptr<GfxDevice> createDevice(const DeviceCreateInfo& createInfo) = 0;
    };
} // namespace rvk

/* Function Declaration */
