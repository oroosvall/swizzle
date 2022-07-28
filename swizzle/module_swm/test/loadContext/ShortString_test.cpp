
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

namespace shortStringNew
{

    TEST(SWM_LoadShortString, shortStringOk)
    {
        std::stringstream ss;
        char data[] = { 'f', 'o', 'o' };
        U8 count = sizeof(data);
        ss.write((char*)&count, 1);
        ss.write(data, count);

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        std::string str;
        EXPECT_EQ(ldr.loadShortString(str), swm::Result::Success);
        EXPECT_STREQ(str.c_str(), "foo");
    }

    TEST(SWM_LoadShortString, shortStringToFewChars)
    {
        std::stringstream ss;
        char data[] = "foo";
        U8 count = 10u;
        ss.write((char*)&count, 1);
        ss.write(data, sizeof(data));

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        std::string str;
        EXPECT_EQ(ldr.loadShortString(str), swm::Result::ErrFileIo);
    }

    TEST(SWM_LoadShortString, failToReadSize)
    {
        std::stringstream ss;

        swm::file::FileReader fr(ss);
        swm::load::LoaderCommon ldr(fr);

        std::string str;

        EXPECT_EQ(ldr.loadShortString(str), swm::Result::ErrFileIo);
    }

}


/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
