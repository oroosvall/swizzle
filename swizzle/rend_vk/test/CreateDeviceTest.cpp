/* Include files */

#include <gtest/gtest.h>

#include "mock/VkInstanceMock.hpp"

#include "rend_vk/RendVk.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

/// <summary>
/// Test checking that device is able to be created
/// the device returned from creation must be non nullptr
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
TEST(rvk_Device, createDevice_Ok)
{
    PhysicalDeviceMockInfo pDev1{};
    pDev1.mName                                           = "TEST_DEVICE_1";
    pDev1.mExtensions                                     = {};
    pDev1.mMaintenace13Features.maintenance4              = true;
    pDev1.mPhysicalDeviceFeatures.pipelineStatisticsQuery = true;
    pDev1.mPhysicalDeviceFeatures.fillModeNonSolid        = true;

    VkInstanceMockInitialize(InstanceMockInit::InstanceValid, {pDev1});

    auto context = rvk::VkRendCreateContext(true);
    EXPECT_NE(context, nullptr);
    EXPECT_EQ(context->getDeviceCount(), 1u);

    rvk::DeviceCreateInfo info{};
    info.mDeviceIndex   = 0u;
    info.mGenericQueues = {1.0f};

    auto device = context->createDevice(info);
    EXPECT_NE(device, nullptr);
    EXPECT_STREQ(device->getDeviceName().c_str(), "TEST_DEVICE_1");

    device.reset();
    context.reset();

    VkInstanceMockCleanup();
}
/// <summary>
/// Test that the device returned is nullptr
/// when not all required features are present
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
TEST(rvk_Device, createDevice_MissingRequiredFeatures)
{
    PhysicalDeviceMockInfo pDev1{};
    pDev1.mName                                           = "TEST_DEVICE_1";
    pDev1.mExtensions                                     = {};
    pDev1.mMaintenace13Features.maintenance4              = false;
    pDev1.mPhysicalDeviceFeatures.pipelineStatisticsQuery = true;
    pDev1.mPhysicalDeviceFeatures.fillModeNonSolid        = true;

    VkInstanceMockInitialize(InstanceMockInit::InstanceValid, {pDev1});

    auto context = rvk::VkRendCreateContext(true);
    EXPECT_NE(context, nullptr);
    EXPECT_EQ(context->getDeviceCount(), 1u);

    rvk::DeviceCreateInfo info{};
    info.mDeviceIndex   = 0u;
    info.mGenericQueues = {1.0f};

    auto device = context->createDevice(info);
    EXPECT_EQ(device, nullptr);

    context.reset();

    VkInstanceMockCleanup();
}

/// <summary>
/// Test that the device returned is nullptr
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
TEST(rvk_Device, createDevice_MissingDesiredFeatures)
{
    PhysicalDeviceMockInfo pDev1{};
    pDev1.mName                                           = "TEST_DEVICE_1";
    pDev1.mExtensions                                     = {};
    pDev1.mMaintenace13Features.maintenance4              = true;
    pDev1.mPhysicalDeviceFeatures.pipelineStatisticsQuery = true;
    pDev1.mPhysicalDeviceFeatures.fillModeNonSolid        = true;

    VkInstanceMockInitialize(InstanceMockInit::InstanceValid, {pDev1});

    auto context = rvk::VkRendCreateContext(true);
    EXPECT_NE(context, nullptr);
    EXPECT_EQ(context->getDeviceCount(), 1u);

    rvk::DeviceCreateInfo info{};
    info.mDeviceIndex                  = 0u;
    info.mGenericQueues                = {1.0f};
    info.mDesiredFeatures.mTesselation = rvk::FeatureState::Required;

    auto device = context->createDevice(info);
    EXPECT_EQ(device, nullptr);

    context.reset();

    VkInstanceMockCleanup();
}

/// <summary>
/// Test that device is successfully created when desired features are available
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
TEST(rvk_Device, createDevice_HasDesiredFeatures)
{
    PhysicalDeviceMockInfo pDev1{};
    pDev1.mName                                           = "TEST_DEVICE_1";
    pDev1.mExtensions                                     = {};
    pDev1.mMaintenace13Features.maintenance4              = true;
    pDev1.mPhysicalDeviceFeatures.pipelineStatisticsQuery = true;
    pDev1.mPhysicalDeviceFeatures.fillModeNonSolid        = true;
    pDev1.mPhysicalDeviceFeatures.tessellationShader      = true;

    VkInstanceMockInitialize(InstanceMockInit::InstanceValid, {pDev1});

    auto context = rvk::VkRendCreateContext(true);
    EXPECT_NE(context, nullptr);
    EXPECT_EQ(context->getDeviceCount(), 1u);

    rvk::DeviceCreateInfo info{};
    info.mDeviceIndex                  = 0u;
    info.mGenericQueues                = {1.0f};
    info.mDesiredFeatures.mTesselation = rvk::FeatureState::Required;

    auto device = context->createDevice(info);
    EXPECT_NE(device, nullptr);
    EXPECT_STREQ(device->getDeviceName().c_str(), "TEST_DEVICE_1");

    device.reset();
    context.reset();

    VkInstanceMockCleanup();
}