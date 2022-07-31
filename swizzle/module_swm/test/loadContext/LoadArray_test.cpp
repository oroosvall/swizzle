
/* Include files */

#include <gtest/gtest.h>

#include "Load.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace loadArray
{
    TEST(SWM_LoadArray, readArrayOk)
    {
        const U8 expected[] = { 0, 1, 2, 3, 4 };
        std::stringstream ss;
        ss.write((const char*)&expected, sizeof(expected));

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        U8 num = 5u;
        std::vector<U8> data;

        EXPECT_EQ(ldr.loadArray(data, num), swm::Result::Success);
        EXPECT_EQ(data.size(), num);
    }


    TEST(SWM_LoadArray, failToReadArray)
    {
        std::stringstream ss;

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        U8 num = 5u;
        std::vector<U8> data;

        EXPECT_EQ(ldr.loadArray(data, num), swm::Result::ErrFileIo);
        EXPECT_EQ(data.size(), 0u);
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
