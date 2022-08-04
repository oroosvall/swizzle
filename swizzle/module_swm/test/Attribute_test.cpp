
/* Include files */

#include <gtest/gtest.h>

#include "InternalStructs.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace InternalStructs
{
    namespace stcf = swm::types::compressFlags;

    TEST(SWM_InternalStructs, getUnusedAttribute)
    {
        auto attribs = swm::types::GetAttributes(0x40);

        EXPECT_EQ(attribs.size(), 1u);
        EXPECT_EQ(attribs[0u], stcf::CompressedChannelAttribute::Unused);
    }

    TEST(SWM_InternalStructs, getCompressAttribute)
    {
        auto attribs = swm::types::GetAttributes(0x80);

        EXPECT_EQ(attribs.size(), 1u);
        EXPECT_EQ(attribs[0u], stcf::CompressedChannelAttribute::Compressed);
    }

    TEST(SWM_InternalStructs, getAllAttributes)
    {
        auto attribs = swm::types::GetAttributes(0xFF);

        EXPECT_EQ(attribs.size(), 8u);
        EXPECT_EQ(attribs[0u], stcf::CompressedChannelAttribute::Position);
        EXPECT_EQ(attribs[1u], stcf::CompressedChannelAttribute::Uv);
        EXPECT_EQ(attribs[2u], stcf::CompressedChannelAttribute::Normal);
        EXPECT_EQ(attribs[3u], stcf::CompressedChannelAttribute::Color);
        EXPECT_EQ(attribs[4u], stcf::CompressedChannelAttribute::BoneIndex);
        EXPECT_EQ(attribs[5u], stcf::CompressedChannelAttribute::BoneWeight);
        EXPECT_EQ(attribs[6u], stcf::CompressedChannelAttribute::Unused);
        EXPECT_EQ(attribs[7u], stcf::CompressedChannelAttribute::Compressed);
    }

    TEST(SWM_InternalStructs, getPositionChannelSize)
    {
        U64 outSize = 0ull;
        auto res = swm::types::GetChannelSize(0x01, outSize);

        EXPECT_EQ(res, swm::Result::Success);
        EXPECT_EQ(outSize, sizeof(swm::types::Vector3F));
    }

    TEST(SWM_InternalStructs, getUvChannelSize)
    {
        U64 outSize = 0ull;
        auto res = swm::types::GetChannelSize(0x02, outSize);

        EXPECT_EQ(res, swm::Result::Success);
        EXPECT_EQ(outSize, sizeof(swm::types::Vector2F));
    }

    TEST(SWM_InternalStructs, getColorChannelSize)
    {
        U64 outSize = 0ull;
        auto res = swm::types::GetChannelSize(0x08, outSize);

        EXPECT_EQ(res, swm::Result::Success);
        EXPECT_EQ(outSize, sizeof(swm::types::Color4U));
    }

    TEST(SWM_InternalStructs, getBoneIndexChannelSize)
    {
        U64 outSize = 0ull;
        auto res = swm::types::GetChannelSize(0x10, outSize);

        EXPECT_EQ(res, swm::Result::Success);
        EXPECT_EQ(outSize, sizeof(swm::types::BoneIndex));
    }

    TEST(SWM_InternalStructs, getBoneWeightChannelSize)
    {
        U64 outSize = 0ull;
        auto res = swm::types::GetChannelSize(0x20, outSize);

        EXPECT_EQ(res, swm::Result::Success);
        EXPECT_EQ(outSize, sizeof(swm::types::BoneWeights));
    }

    TEST(SWM_InternalStructs, ChannelSize_toMany)
    {
        U64 outSize = 0ull;
        auto res = swm::types::GetChannelSize(0x22, outSize);

        EXPECT_EQ(res, swm::Result::ErrToManyAttributes);
        EXPECT_EQ(outSize, 0ull);
    }

    TEST(SWM_InternalStructs, ChannelSize_RemoveCompress)
    {
        U64 outSize = 0ull;
        auto res = swm::types::GetChannelSize(0x82, outSize);

        EXPECT_EQ(res, swm::Result::Success);
        EXPECT_EQ(outSize, sizeof(swm::types::Vector2F));
    }

    TEST(SWM_InternalStructs, AttributeSize_Default)
    {
        EXPECT_EQ(swm::types::GetSizeOfAttribute(static_cast<stcf::CompressedChannelAttribute>(12)), 0ull);
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
