#include <gtest/gtest.h>
#include <common/common.hpp>
#include <utils/BufferReader.hpp>

#include "Internal.hpp"

TEST(SWM_Magic, checkNullptr)
{
    ASSERT_EQ(swm_int::CheckMagic(nullptr), false);
}

TEST(SWM_Magic, invalidMagic)
{
    swm_int::Magic m;

    m.mMagic[0] = 0;
    m.mMagic[1] = 1;
    m.mMagic[2] = 2;
    m.mMagic[3] = 3;

    EXPECT_EQ(swm_int::CheckMagic(&m), false);
}

TEST(SWM_Magic, correctMagic)
{
    swm_int::Magic m;

    m.mMagic[0] = 'S';
    m.mMagic[1] = 'W';
    m.mMagic[2] = 'M';
    m.mMagic[3] = ' ';

    EXPECT_EQ(swm_int::CheckMagic(&m), true);
}