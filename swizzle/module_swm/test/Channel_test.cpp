
/* Include files */

#include <gtest/gtest.h>

#include "ChannelCommon.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

TEST(SWM_Channel, checkWrapping)
{
    EXPECT_EQ(swm::channel::GetChannelElemetSize(0), 1ull);
    EXPECT_EQ(swm::channel::GetChannelElemetSize(1), 2ull);
    EXPECT_EQ(swm::channel::GetChannelElemetSize(2), 4ull);
    EXPECT_EQ(swm::channel::GetChannelElemetSize(3), 8ull);
    EXPECT_EQ(swm::channel::GetChannelElemetSize(4), 1ull);
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
