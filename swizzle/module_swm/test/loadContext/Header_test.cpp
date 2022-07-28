
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

namespace readHeader
{
    TEST(SWM_Load_Header, readHeaderOk)
    {
        std::string str = "SWM ,data";
        std::stringstream ss(str);

        swm::file::FileReader fr(ss);
        swm::types::Header hdr{};
        EXPECT_EQ(swm::load::LoadHeader(fr, hdr), swm::Result::Success);
    }

    TEST(SWM_Load_Header, cannotReadHeader)
    {
        std::string str = "foo";
        std::stringstream ss(str);

        swm::file::FileReader fr(ss);
        swm::types::Header hdr{};
        EXPECT_EQ(swm::load::LoadHeader(fr, hdr), swm::Result::ErrFileIo);
    }
}

namespace validateHeader
{
    TEST(SWM_Load_Header, validateHeaderOk)
    {
        swm::types::Header hdr{};
        hdr.mMagic[0] = 'S';
        hdr.mMagic[1] = 'W';
        hdr.mMagic[2] = 'M';
        hdr.mMagic[3] = ' ';
        EXPECT_EQ(swm::load::ValidateHeaderMagic(hdr), swm::Result::Success);
    }

    TEST(SWM_Load_Header, validateHeaderFail)
    {
        swm::types::Header hdr{};
        hdr.mMagic[0] = 'F';
        hdr.mMagic[1] = 'O';
        hdr.mMagic[2] = 'O';
        hdr.mMagic[3] = ' ';
        EXPECT_EQ(swm::load::ValidateHeaderMagic(hdr), swm::Result::ErrMagic);
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
