
#pragma once

/* Include files */

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

/* Defines */

/* Typedefs/enums */

enum class InstanceMockInit
{
    InstanceInvalid,
    InstanceInvalid_NoExtensions,
    InstanceValid
};

/* Forward Declared Structs/Classes */

/* Struct Declaration */

struct PhysicalDeviceMockInfo
{
    std::string mName;
    std::vector<const char*> mExtensions;

    VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;
    // sType and pNext does not need to be set, for mocking
    VkPhysicalDeviceVulkan13Features mMaintenace13Features;
    // sType and pNext does not need to be set, for mocking
    VkPhysicalDeviceMeshShaderFeaturesEXT mMeshShaderFeatures;
    // sType and pNext does not need to be set, for mocking
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR mRayTracingFeatures;
};

/* Class Declaration */

/* Function Declaration */

void VkInstanceMockInitialize(InstanceMockInit init, std::vector<PhysicalDeviceMockInfo> infos);
void VkInstanceMockCleanup();
