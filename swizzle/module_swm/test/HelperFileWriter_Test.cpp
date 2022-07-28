
/* Include files */

#include "helpers/FileWriter.hpp"

#include <gtest/gtest.h>


/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

TEST(SWM_Helpers_FileWriter, write)
{
    std::stringstream ss;
    swm::file::FileWriter fw(ss);

    fw.write('a');
    EXPECT_EQ(ss.str().size(), (size_t)1);
}

TEST(SWM_Helpers_FileWriter, writeArray)
{
    std::stringstream ss;
    swm::file::FileWriter fw(ss);

    std::string str = "foo";

    fw.writeArray(str.data(), str.size());
    EXPECT_EQ(ss.str().size(), str.size());
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
