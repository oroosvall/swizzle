#include <gtest/gtest.h>
#include <utils/Bitstream.hpp>

TEST(Bitstream, writeAlternating)
{
    const uint8_t expected[] = {0b10101010, 0b10101010, 0b10101010, 0b10101010,
                                0b10101010, 0b10101010, 0b10101010, 0b10101010};
    uint64_t data;

    utils::BitStreamWriter bsw((uint8_t*)&data, sizeof(data));

    for (size_t i = 0U; i < sizeof(data) * 4U; i++)
    {
        bsw.writeBits(0b1U, 1U);
        bsw.writeBits(0b0U, 1U);
    }

    bsw.flush();

    EXPECT_EQ(memcmp(expected, &data, sizeof(data)), 0);
}

TEST(Bitstream, writeAlternating2bits)
{
    const uint8_t expected[] = {0b11001100, 0b11001100, 0b11001100, 0b11001100,
                                0b11001100, 0b11001100, 0b11001100, 0b11001100};
    uint64_t data;

    utils::BitStreamWriter bsw((uint8_t*)&data, sizeof(data));

    for (size_t i = 0U; i < sizeof(data) * 2U; i++)
    {
        bsw.writeBits(0b11U, 2U);
        bsw.writeBits(0b00U, 2U);
    }

    bsw.flush();

    EXPECT_EQ(memcmp(expected, &data, sizeof(data)), 0);
}

TEST(Bitstream, writeAlternating3bits)
{
    const uint8_t expected[] = {0b11100011, 0b10001110, 0b00111000, 0b11100011,
                                0b10001110, 0b00111000, 0b11100011, 0b10000000};
    uint64_t data;

    utils::BitStreamWriter bsw((uint8_t*)&data, sizeof(data));

    for (size_t i = 0U; i < 10; i++)
    {
        bsw.writeBits(0b111U, 3U);
        bsw.writeBits(0b000U, 3U);
    }

    bsw.flush();

    EXPECT_EQ(memcmp(expected, &data, sizeof(data)), 0);
}

TEST(Bitstream, readAlternating)
{
    const uint32_t firstExpected = 0b1U;
    const uint32_t secondExected = 0b0U;
    uint8_t data[] = {0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010};

    utils::BitStreamReader bsr(data, sizeof(data));

    for (size_t i = 0U; i < sizeof(data) * 4U; i++)
    {
        uint32_t first = 0U;
        uint32_t second = 0U;

        bsr.readBits(first, 1U);
        bsr.readBits(second, 1U);

        EXPECT_EQ(first, firstExpected);
        EXPECT_EQ(second, secondExected);
    }
}

TEST(Bitstream, readAlternating2bits)
{
    const uint32_t firstExpected = 0b11U;
    const uint32_t secondExected = 0b00U;
    uint8_t data[] = {0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100};

    utils::BitStreamReader bsr(data, sizeof(data));

    for (size_t i = 0U; i < sizeof(data) * 2U; i++)
    {
        uint32_t first = 0U;
        uint32_t second = 0U;

        bsr.readBits(first, 2U);
        bsr.readBits(second, 2U);

        EXPECT_EQ(first, firstExpected);
        EXPECT_EQ(second, secondExected);
    }
}

TEST(Bitstream, readAlternating3bits)
{
    const uint32_t firstExpected = 0b111U;
    const uint32_t secondExected = 0b000U;
    uint8_t data[] = {0b11100011, 0b10001110, 0b00111000, 0b11100011, 0b10001110, 0b00111000, 0b11100011, 0b10000000};

    utils::BitStreamReader bsr(data, sizeof(data));

    for (size_t i = 0U; i < 10; i++)
    {
        uint32_t first = 0U;
        uint32_t second = 0U;

        bsr.readBits(first, 3U);
        bsr.readBits(second, 3U);

        EXPECT_EQ(first, firstExpected);
        EXPECT_EQ(second, secondExected);
    }
}

TEST(Bitstream, readAlternating10bits)
{
    const uint32_t firstExpected = 0b1111111111U;
    const uint32_t secondExected = 0b0000000000U;
    uint8_t data[] = {0b11111111, 0b11000000, 0b00001111, 0b11111100, 0b00000000, 0b11111111, 0b11000000,
                      0b00001111, 0b11111100, 0b00000000, 0b11111111, 0b11000000, 0b00001111, 0b11111100,
                      0b00000000, 0b11111111, 0b11000000, 0b00001111, 0b11111100, 0b00000000};

    utils::BitStreamReader bsr(data, sizeof(data));

    for (size_t i = 0U; i < 6; i++)
    {
        uint32_t first = 0U;
        uint32_t second = 0U;

        bsr.readBits(first, 10U);
        bsr.readBits(second, 10U);

        EXPECT_EQ(first, firstExpected);
        EXPECT_EQ(second, secondExected);
    }
}
