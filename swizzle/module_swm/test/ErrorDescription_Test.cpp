
/* Include files */

#include <gtest/gtest.h>

#include <swm/Swm.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

TEST(SWM_Err, validResult)
{
    EXPECT_STREQ(swm::GetErrorDescription(swm::Result::Success), "Success");
}

TEST(SWM_Err, invalidResult)
{
    EXPECT_STREQ(swm::GetErrorDescription(static_cast<swm::Result>(-1)), "Unknown result");
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
