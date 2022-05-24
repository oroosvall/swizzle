#include <gtest/gtest.h>
#include <common/Common.hpp>
#include <utils/BufferReader.hpp>

#include "load/LoadBaseTypes.hpp"

TEST(SWM_Base, loadString_success)
{
    std::vector<uint8_t> data
    {
        0x5, 'h', 'e', 'l', 'l', 'o', '\0'
    };

    utils::BufferReader br(data);

    std::string outString = "";

    EXPECT_EQ(swm_base::LoadString(br, outString), swm::LoadResult::Success);
    EXPECT_STREQ(outString.c_str(), "hello");
}

TEST(SWM_Base, loadString_stringError)
{
    std::vector<uint8_t> data
    {
        0x12, 'h', 'e', 'l', 'l', 'o', '\0'
    };

    utils::BufferReader br(data);

    std::string outString = "";

    EXPECT_EQ(swm_base::LoadString(br, outString), swm::LoadResult::StringError);
    EXPECT_STREQ(outString.c_str(), "");
}

TEST(SWM_Base, resultIsOk_true)
{
    EXPECT_TRUE(swm_base::ResultIsOk(swm::LoadResult::Success));
}

TEST(SWM_Base, resultIsOk_false)
{
    EXPECT_FALSE(swm_base::ResultIsOk(swm::LoadResult::FileIoError));
}

TEST(SWM_Base, readNumber_U8_ok)
{
    std::vector<uint8_t> data
    {
        0x1, 0x2
    };

    utils::BufferReader br(data);
    U8 num = 0u;
    swm::LoadResult lr = swm_base::LoadNumber<U8, swm::LoadResult::SizeError>(br, num);
    EXPECT_EQ(lr, swm::LoadResult::Success);
}

TEST(SWM_Base, readNumber_U8_error)
{
    std::vector<uint8_t> data;
    utils::BufferReader br(data);
    U8 num = 0u;
    swm::LoadResult lr = swm_base::LoadNumber<U8, swm::LoadResult::SizeError>(br, num);
    EXPECT_EQ(lr, swm::LoadResult::SizeError);
}

TEST(SWM_Base, readArray_ok)
{
    std::vector<uint8_t> data
    {
        0x1, 0x2
    };

    utils::BufferReader br(data);
    std::vector<U8> res;
    swm::LoadResult lr = swm_base::LoadArray<U8, swm::LoadResult::SizeError>(br, res, 2);
    EXPECT_EQ(lr, swm::LoadResult::Success);
}

TEST(SWM_Base, readArray_error)
{
    std::vector<uint8_t> data
    {
        0x1, 0x2
    };

    utils::BufferReader br(data);
    std::vector<U8> res;
    swm::LoadResult lr = swm_base::LoadArray<U8, swm::LoadResult::VertexError>(br, res, 3);
    EXPECT_EQ(lr, swm::LoadResult::VertexError);
}