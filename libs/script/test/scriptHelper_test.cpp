#include <gtest/gtest.h>
#include <compiler/ScriptHelperFuncs.hpp>

TEST(ScriptTests_helpers, signedNumberCheck)
{

    script::langString numberString = "123456";
    script::langString unsignedNumberString = "123456U";
    script::langString negativeNumberString = "-12351";
    script::langString negativeUnsignedNumberString = "-12351u";

    EXPECT_TRUE(script::isSignedNumber(numberString));
    // we really want negative numbers to fail
    // the '-' token should be separated by the tokenizer
    EXPECT_FALSE(script::isSignedNumber(negativeNumberString));

    EXPECT_FALSE(script::isSignedNumber(unsignedNumberString));
    EXPECT_FALSE(script::isSignedNumber(negativeUnsignedNumberString));
}

TEST(ScriptTests_helpers, unsignedNumberCheck)
{

    script::langString numberString = "123456";
    script::langString unsignedNumberString = "123456U";
    script::langString negativeNumberString = "-12351";
    script::langString negativeUnsignedNumberString = "-12351u";

    EXPECT_FALSE(script::isUnsignedNumber(numberString));
    EXPECT_FALSE(script::isUnsignedNumber(negativeNumberString));

    EXPECT_TRUE(script::isUnsignedNumber(unsignedNumberString));
    // we really want negative numbers to fail
    // the '-' token should be separated by the tokenizer
    EXPECT_FALSE(script::isUnsignedNumber(negativeUnsignedNumberString));
}

TEST(ScriptTests_helpers, floatNumberCheck)
{

    script::langString numberString = "123456";
    script::langString doubleNumberString = "1.0";
    script::langString floatNumberString = "0.0F";
    script::langString negativeDoubleNumberString = "-0.0";
    script::langString negativeFloatNumberString = "-0.0f";

    EXPECT_FALSE(script::isFloatNumber(numberString));
    EXPECT_FALSE(script::isFloatNumber(doubleNumberString));

    EXPECT_TRUE(script::isFloatNumber(floatNumberString));
    // we really want negative numbers to fail
    // the '-' token should be separated by the tokenizer
    EXPECT_FALSE(script::isFloatNumber(negativeDoubleNumberString));
    EXPECT_FALSE(script::isFloatNumber(negativeFloatNumberString));
}

TEST(ScriptTests_helpers, doubleNumberCheck)
{

    script::langString numberString = "123456";
    script::langString doubleNumberString = "1.0";
    script::langString floatNumberString = "0.0F";
    script::langString negativeDoubleNumberString = "-0.0";
    script::langString negativeFloatNumberString = "-0.0f";

    EXPECT_FALSE(script::isDoubleNumber(numberString));
    EXPECT_TRUE(script::isDoubleNumber(doubleNumberString));

    EXPECT_FALSE(script::isDoubleNumber(floatNumberString));
    // we really want negative numbers to fail
    // the '-' token should be separated by the tokenizer
    EXPECT_FALSE(script::isDoubleNumber(negativeDoubleNumberString));
    EXPECT_FALSE(script::isDoubleNumber(negativeFloatNumberString));
}

TEST(ScriptTests_helpers, stringCheck)
{

    script::langString quoteTextString = "\"Foo\"";
    script::langString normalText = "Foo";
    script::langString numberString = "123";

    EXPECT_TRUE(script::isString(quoteTextString));

    EXPECT_FALSE(script::isString(normalText));
    EXPECT_FALSE(script::isString(numberString));
}