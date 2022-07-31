
/* Include files */

#include "helpers/FileReader.hpp"

#include <gtest/gtest.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

TEST(SWM_Helpers_FileReader, readOk)
{
    //std::stringstream ss("foo, bar", std::ios::binary);
    std::string str ="foo, bar";
    std::stringstream ss(str);

    swm::file::FileReader fr(ss);

    char f = 0x0;
    EXPECT_TRUE(fr.readNum(f));
    EXPECT_EQ(f, 'f');
    EXPECT_EQ(fr.getReadCount(), (size_t)1);

    std::vector<char> v;
    EXPECT_TRUE(fr.readArray(v, 3));
    EXPECT_EQ(v[0], 'o');
    EXPECT_EQ(v[1], 'o');
    EXPECT_EQ(v[2], ',');
    EXPECT_EQ(fr.getReadCount(), (size_t)4);
}

TEST(SWM_Helpers_FileReader, readOutOfSpace)
{
    //std::stringstream ss("foo, bar", std::ios::binary);
    std::string str = "foo";
    std::stringstream ss(str);

    swm::file::FileReader fr(ss);

    std::vector<char> v;
    EXPECT_FALSE(fr.readArray(v, 20));
    EXPECT_EQ(fr.getReadCount(), (size_t)0);
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */

