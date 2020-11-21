
#include <gtest/gtest.h>
#include "runtime/ProgramContext.hpp"

TEST(ScriptTests_Runtime_Reg, Add_SrcA_DstA_AsInt)
{
    OpFlags flags = flagsToOpFlags(0x00);

    ASSERT_EQ(flags.mFloating, false);
    ASSERT_EQ(flags.mSrcAsValue, false);

    ASSERT_EQ(flags.mSrcReg, Registers::AReg);
    ASSERT_EQ(flags.mDstReg, Registers::AReg);
}

TEST(ScriptTests_Runtime_Reg, Add_SrcA_DstA_AsFloat)
{
    OpFlags flags = flagsToOpFlags(0x80);

    ASSERT_EQ(flags.mFloating, true);
    ASSERT_EQ(flags.mSrcAsValue, false);

    ASSERT_EQ(flags.mSrcReg, Registers::AReg);
    ASSERT_EQ(flags.mDstReg, Registers::AReg);
}

TEST(ScriptTests_Runtime_Reg, Add_SrcA_DstB_AsInt)
{
    OpFlags flags = flagsToOpFlags(0x10);

    ASSERT_EQ(flags.mFloating, false);
    ASSERT_EQ(flags.mSrcAsValue, false);

    ASSERT_EQ(flags.mSrcReg, Registers::AReg);
    ASSERT_EQ(flags.mDstReg, Registers::BReg);
}

TEST(ScriptTests_Runtime_Reg, Add_SrcA_DstB_AsFloat)
{
    OpFlags flags = flagsToOpFlags(0x90);

    ASSERT_EQ(flags.mFloating, true);
    ASSERT_EQ(flags.mSrcAsValue, false);

    ASSERT_EQ(flags.mSrcReg, Registers::AReg);
    ASSERT_EQ(flags.mDstReg, Registers::BReg);
}

TEST(ScriptTests_Runtime_Reg, Add_SrcValue_DstB_AsInt)
{
    OpFlags flags = flagsToOpFlags(0x1F);

    ASSERT_EQ(flags.mFloating, false);
    ASSERT_EQ(flags.mSrcAsValue, true);

    ASSERT_EQ(flags.mSrcReg, Registers::REG_END);
    ASSERT_EQ(flags.mDstReg, Registers::BReg);
}

TEST(ScriptTests_Runtime_Reg, Add_SrcValue_DstB_AsFloat)
{
    OpFlags flags = flagsToOpFlags(0x9F);

    ASSERT_EQ(flags.mFloating, true);
    ASSERT_EQ(flags.mSrcAsValue, true);

    ASSERT_EQ(flags.mSrcReg, Registers::REG_END);
    ASSERT_EQ(flags.mDstReg, Registers::BReg);
}


TEST(ScriptTests_Runtime_Reg, flagsConvert_1)
{
    OpFlags inFlags;
    inFlags.mFloating = false;
    inFlags.mSrcAsValue = false;
    inFlags.mSrcReg = Registers::AReg;
    inFlags.mDstReg = Registers::AReg;

    uint8_t out = opFlagsToFlags(inFlags);
    OpFlags flags = flagsToOpFlags(out);

    ASSERT_EQ(flags.mFloating, false);
    ASSERT_EQ(flags.mSrcAsValue, false);

    ASSERT_EQ(flags.mSrcReg, Registers::AReg);
    ASSERT_EQ(flags.mDstReg, Registers::AReg);
}

TEST(ScriptTests_Runtime_Reg, flagsConvert_2)
{
    OpFlags inFlags;
    inFlags.mFloating = true;
    inFlags.mSrcAsValue = false;
    inFlags.mSrcReg = Registers::BReg;
    inFlags.mDstReg = Registers::DReg;

    uint8_t out = opFlagsToFlags(inFlags);
    OpFlags flags = flagsToOpFlags(out);

    ASSERT_EQ(flags.mFloating, true);
    ASSERT_EQ(flags.mSrcAsValue, false);

    ASSERT_EQ(flags.mSrcReg, Registers::BReg);
    ASSERT_EQ(flags.mDstReg, Registers::DReg);
}

TEST(ScriptTests_Runtime_Reg, flagsConvert_3)
{
    OpFlags inFlags;
    inFlags.mFloating = true;
    inFlags.mSrcAsValue = true;
    inFlags.mSrcReg = Registers::BReg;
    inFlags.mDstReg = Registers::DReg;

    uint8_t out = opFlagsToFlags(inFlags);
    OpFlags flags = flagsToOpFlags(out);

    ASSERT_EQ(flags.mFloating, true);
    ASSERT_EQ(flags.mSrcAsValue, true);

    ASSERT_EQ(flags.mSrcReg, Registers::REG_END);
    ASSERT_EQ(flags.mDstReg, Registers::DReg);
}