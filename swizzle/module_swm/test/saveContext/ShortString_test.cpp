
/* Include files */

#include <gtest/gtest.h>

#include "Save.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace shortString
{
    TEST(SWM_SaveShortString, shortStringOk)
    {
        std::stringstream ss;

        swm::file::FileWriter fw(ss);
        swm::save::SaverCommon svr(fw);

        std::string str = "Foo";
        svr.saveShortString(str);

        EXPECT_EQ(ss.str().size(), 4u);
    }

    TEST(SWM_SaveShortString, shortStringTruncate)
    {
        std::stringstream ss;

        swm::file::FileWriter fw(ss);
        swm::save::SaverCommon svr(fw);

        std::string str = "";

        for(size_t i = 0u; i < 300; ++i)
        {
            str += "a";
        }
        
        svr.saveShortString(str);

        EXPECT_EQ(ss.str().size(), 256u);
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */

