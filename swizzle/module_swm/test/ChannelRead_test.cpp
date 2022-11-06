
/* Include files */

#include <gtest/gtest.h>

#include "ChannelCommon.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

TEST(SWM_ChannelRead, ReadVector2)
{
    F32 fData[2] = {0.0f, 1.0f};
    U8 idx = 0b00011011;

    swm::channel::CompressedChannel ch{};
    ch.mData.resize(sizeof(fData));
    memcpy(ch.mData.data(), fData, sizeof(fData));
    ch.mBitsPerIndex = 1u;
    ch.mElementCount = 2u;

    utils::BitStreamReader bsr{&idx, 1};

    auto v = swm::channel::ReadVector2(ch, bsr);
    EXPECT_FLOAT_EQ(v.u, fData[0]);
    EXPECT_FLOAT_EQ(v.v, fData[0]);

    v = swm::channel::ReadVector2(ch, bsr);
    EXPECT_FLOAT_EQ(v.u, fData[0]);
    EXPECT_FLOAT_EQ(v.v, fData[1]);

    v = swm::channel::ReadVector2(ch, bsr);
    EXPECT_FLOAT_EQ(v.u, fData[1]);
    EXPECT_FLOAT_EQ(v.v, fData[0]);

    v = swm::channel::ReadVector2(ch, bsr);
    EXPECT_FLOAT_EQ(v.u, fData[1]);
    EXPECT_FLOAT_EQ(v.v, fData[1]);
}

TEST(SWM_ChannelRead, ReadVector3)
{
    F32 fData[3] = {0.0f, 1.0f, 2.0f};
    U8 idx = 0b00011011;

    swm::channel::CompressedChannel ch{};
    ch.mData.resize(sizeof(fData));
    memcpy(ch.mData.data(), fData, sizeof(fData));
    ch.mBitsPerIndex = 2u;
    ch.mElementCount = 3u;

    utils::BitStreamReader bsr{&idx, 1};

    // read 0.0, 1.0, 2.0
    auto v = swm::channel::ReadVector3(ch, bsr);
    EXPECT_FLOAT_EQ(v.x, fData[0]);
    EXPECT_FLOAT_EQ(v.y, fData[1]);
    EXPECT_FLOAT_EQ(v.z, fData[2]);

    // read 0.0, 0.0, 0.0 due to wrapping
    v = swm::channel::ReadVector3(ch, bsr);
    EXPECT_FLOAT_EQ(v.x, fData[0]);
    EXPECT_FLOAT_EQ(v.y, fData[0]);
    EXPECT_FLOAT_EQ(v.z, fData[0]);
}

TEST(SWM_ChannelRead, ReadColor4U_Mapping1)
{
    U8 fData[4] = {12, 13, 14, 15};
    U8 idx = 0b00011011;

    swm::channel::CompressedChannel ch{};
    ch.mData.resize(sizeof(fData));
    memcpy(ch.mData.data(), fData, sizeof(fData));
    ch.mBitsPerIndex = 2u;
    ch.mElementCount = 4u;

    utils::BitStreamReader bsr{&idx, 1};

    auto c = swm::channel::ReadColor4U(ch, bsr, swm::types::mappingFlags::OneToOne);
    EXPECT_EQ(c.r, 12);
    EXPECT_EQ(c.g, 13);
    EXPECT_EQ(c.b, 14);
    EXPECT_EQ(c.a, 15);
}

TEST(SWM_ChannelRead, ReadColor4U_Mapping2)
{
    U8 fData[4] = { 12, 13, 14, 15 };
    U8 idx = 0b00011011;

    swm::channel::CompressedChannel ch{};
    ch.mData.resize(sizeof(fData));
    memcpy(ch.mData.data(), fData, sizeof(fData));
    ch.mBitsPerIndex = 1u;
    ch.mElementCount = 2u;

    utils::BitStreamReader bsr{ &idx, 1 };

    auto c = swm::channel::ReadColor4U(ch, bsr, swm::types::mappingFlags::OneToTwo);
    EXPECT_EQ(c.r, 12);
    EXPECT_EQ(c.g, 13);
    EXPECT_EQ(c.b, 12);
    EXPECT_EQ(c.a, 13);

    c = swm::channel::ReadColor4U(ch, bsr, swm::types::mappingFlags::OneToTwo);
    EXPECT_EQ(c.r, 12);
    EXPECT_EQ(c.g, 13);
    EXPECT_EQ(c.b, 14);
    EXPECT_EQ(c.a, 15);

    c = swm::channel::ReadColor4U(ch, bsr, swm::types::mappingFlags::OneToTwo);
    EXPECT_EQ(c.r, 14);
    EXPECT_EQ(c.g, 15);
    EXPECT_EQ(c.b, 12);
    EXPECT_EQ(c.a, 13);

    c = swm::channel::ReadColor4U(ch, bsr, swm::types::mappingFlags::OneToTwo);
    EXPECT_EQ(c.r, 14);
    EXPECT_EQ(c.g, 15);
    EXPECT_EQ(c.b, 14);
    EXPECT_EQ(c.a, 15);
}

TEST(SWM_ChannelRead, ReadColor4U_Mapping4)
{
    U8 fData[4] = { 12, 13, 14, 15 };
    U8 idx = 0b01000000;

    swm::channel::CompressedChannel ch{};
    ch.mData.resize(sizeof(fData));
    memcpy(ch.mData.data(), fData, sizeof(fData));
    ch.mBitsPerIndex = 1u;
    ch.mElementCount = 1u;

    utils::BitStreamReader bsr{ &idx, 1 };

    auto c = swm::channel::ReadColor4U(ch, bsr, swm::types::mappingFlags::OneToFour);
    EXPECT_EQ(c.r, 12);
    EXPECT_EQ(c.g, 13);
    EXPECT_EQ(c.b, 14);
    EXPECT_EQ(c.a, 15);

    c = swm::channel::ReadColor4U(ch, bsr, swm::types::mappingFlags::OneToFour);
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 0);
    EXPECT_EQ(c.a, 0);
}

TEST(SWM_ChannelRead, ReadBoneIndex)
{
    U16 fData[4] = { 0, 1, 2, 3 };
    U8 idx = 0b00011011;

    swm::channel::CompressedChannel ch{};
    ch.mData.resize(sizeof(fData));
    memcpy(ch.mData.data(), fData, sizeof(fData));
    ch.mBitsPerIndex = 2u;
    ch.mElementCount = 4u;

    utils::BitStreamReader bsr{ &idx, 1 };

    auto b = swm::channel::ReadBoneIndex(ch, bsr);
    EXPECT_EQ(b.b1, 0);
    EXPECT_EQ(b.b2, 1);
    EXPECT_EQ(b.b3, 2);
    EXPECT_EQ(b.b4, 3);
}

TEST(SWM_ChannelRead, ReadBoneWeights)
{
    F32 fData[4] = { 0.0f, 0.25f, 0.75f, 1.0f };
    U8 idx = 0b00011011;

    swm::channel::CompressedChannel ch{};
    ch.mData.resize(sizeof(fData));
    memcpy(ch.mData.data(), fData, sizeof(fData));
    ch.mBitsPerIndex = 2u;
    ch.mElementCount = 4u;

    utils::BitStreamReader bsr{ &idx, 1 };

    auto b = swm::channel::ReadBoneWeight(ch, bsr);
    EXPECT_FLOAT_EQ(b.w1, 0.0f);
    EXPECT_FLOAT_EQ(b.w2, 0.25f);
    EXPECT_FLOAT_EQ(b.w3, 0.75f);
    EXPECT_FLOAT_EQ(b.w4, 1.0f);
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
