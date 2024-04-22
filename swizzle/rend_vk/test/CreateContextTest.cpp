
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

TEST(rvk_Context, createContext_nullptr)
{
    VkInstanceMockInitialize(InstanceMockInit::InstanceInvalid, {});

    auto context = rvk::VkRendCreateContext(false);
    EXPECT_EQ(context, nullptr);

    VkInstanceMockCleanup();
}

TEST(rvk_Context, createContext_NoExtensions)
{
    VkInstanceMockInitialize(InstanceMockInit::InstanceInvalid_NoExtensions, {});

    auto context = rvk::VkRendCreateContext(false);
    EXPECT_EQ(context, nullptr);

    VkInstanceMockCleanup();
}

TEST(rvk_Context, createContext_valid)
{
    VkInstanceMockInitialize(InstanceMockInit::InstanceValid, {});

    auto context = rvk::VkRendCreateContext(false);
    EXPECT_NE(context, nullptr);
    EXPECT_EQ(context->getDeviceCount(), 1u);

    context.reset();

    VkInstanceMockCleanup();
}

TEST(rvk_Context, createContext_valid_debugCallbacks)
{
    VkInstanceMockInitialize(InstanceMockInit::InstanceValid, {});

    auto context = rvk::VkRendCreateContext(true);
    EXPECT_NE(context, nullptr);
    EXPECT_EQ(context->getDeviceCount(), 1u);

    context.reset();

    VkInstanceMockCleanup();
}
