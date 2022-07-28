
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

namespace loadNumber
{
    TEST(SWM_LoadNumber, readNumOkU8)
    {
        const U8 expected = 0x12;
        std::stringstream ss;
        ss.write((const char*) &expected, sizeof(expected));

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        U8 num = 0u;

        EXPECT_EQ(ldr.loadNumber(num), swm::Result::Success);
        EXPECT_EQ(num, expected);
    }

    TEST(SWM_LoadNumber, readNumOkU16)
    {
        const U16 expected = 0x1234;
        std::stringstream ss;
        ss.write((const char*)&expected, sizeof(expected));

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        U16 num = 0u;

        EXPECT_EQ(ldr.loadNumber(num), swm::Result::Success);
        EXPECT_EQ(num, expected);
    }

    TEST(SWM_LoadNumber, readNumOkU32)
    {
        const U32 expected = 0x12345678;
        std::stringstream ss;
        ss.write((const char*)&expected, sizeof(expected));

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        U32 num = 0u;

        EXPECT_EQ(ldr.loadNumber(num), swm::Result::Success);
        EXPECT_EQ(num, expected);
    }

    TEST(SWM_LoadNumber, failToReadU8)
    {
        std::stringstream ss;

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        U8 num = 0u;

        EXPECT_EQ(ldr.loadNumber(num), swm::Result::ErrFileIo);
    }

    TEST(SWM_LoadNumber, failToReadU16)
    {
        std::stringstream ss;

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        U16 num = 0u;

        EXPECT_EQ(ldr.loadNumber(num), swm::Result::ErrFileIo);
    }

    TEST(SWM_LoadNumber, failToReadU32)
    {
        std::stringstream ss;

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        U32 num = 0u;

        EXPECT_EQ(ldr.loadNumber(num), swm::Result::ErrFileIo);
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
