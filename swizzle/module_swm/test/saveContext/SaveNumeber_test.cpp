
/* Include files */

#include <gtest/gtest.h>

#include "Save.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

namespace saveNumber
{
    class badStream : public std::stringstream
    {
        public:
        badStream() : std::stringstream() {
            setstate(std::ios::badbit);
        }
        ~badStream() {}
    };
}

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace saveNumber
{
    TEST(SWM_saveNumber, writeNumU8Ok)
    {
        std::stringstream ss;

        swm::file::FileWriter fw(ss);
        swm::save::SaverCommon svr(fw);

        U8 num = 0x12;
        swm::Result res = svr.saveNumber(num);

        EXPECT_EQ(res, swm::Result::Success);
        EXPECT_EQ(ss.str().size(), 1u);
    }

    TEST(SWM_saveNumber, writeNumU16Ok)
    {
        std::stringstream ss;

        swm::file::FileWriter fw(ss);
        swm::save::SaverCommon svr(fw);

        U16 num = 0x1234;
        swm::Result res = svr.saveNumber(num);

        EXPECT_EQ(res, swm::Result::Success);
        EXPECT_EQ(ss.str().size(), 2u);
    }

    TEST(SWM_saveNumber, writeNumU32Ok)
    {
        std::stringstream ss;

        swm::file::FileWriter fw(ss);
        swm::save::SaverCommon svr(fw);

        U32 num = 0x12345667;
        swm::Result res = svr.saveNumber(num);

        EXPECT_EQ(res, swm::Result::Success);
        EXPECT_EQ(ss.str().size(), 4u);
    }


    TEST(SWM_saveNumber, writeNumU32BadStream)
    {
        badStream ss;

        swm::file::FileWriter fw(ss);
        swm::save::SaverCommon svr(fw);

        U32 num = 0x12345667;
        swm::Result res = svr.saveNumber(num);

        EXPECT_EQ(res, swm::Result::ErrFileIo);
        EXPECT_EQ(ss.str().size(), 0u);
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
