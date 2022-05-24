#include <gtest/gtest.h>
#include <common/Common.hpp>

#include "VersionSelect.hpp"
#include "load/LoadV1.hpp"
#include "load/LoadV2.hpp"

TEST(SWM_Version, checkVeresion_invalid)
{
    swm_ver::LoadFn fn = swm_ver::FindLoader(0u);
    EXPECT_EQ(fn, nullptr);
}

TEST(SWM_Version, checkVeresion_1_0)
{
    swm_ver::LoadFn fn = swm_ver::FindLoader(1u);
    EXPECT_EQ(fn, &swm_v1::LoadV1);
}


TEST(SWM_Version, checkVeresion_2_0)
{
    swm_ver::LoadFn fn = swm_ver::FindLoader(2u);
    EXPECT_EQ(fn, &swm_v2::LoadV2);
}
