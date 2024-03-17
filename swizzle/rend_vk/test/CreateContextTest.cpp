
/* Include files */

#include <gtest/gtest.h>

//#include "mocks/VkMock.hpp"

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
    auto context = rvk::VkRendCreateContext(false);

    EXPECT_EQ(context, nullptr);
}

