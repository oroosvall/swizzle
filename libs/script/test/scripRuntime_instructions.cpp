#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "runtime/Instructions.hpp"
#include "runtime/ProgramContext.hpp"

MATCHER_P(RegIntMatcher, v, "") { (void)result_listener; return arg.uVal == v.uVal; }
MATCHER_P(RegFloatMatcher, v, "") { (void)result_listener; return arg.fVal == v.fVal; }

class ProgramContextMock : public ProgramContext
{
public:
    ProgramContextMock() : ProgramContext(10U) {}
    virtual ~ProgramContextMock() {}

    MOCK_METHOD(bool, readRegister, (const Registers, Reg&), (const, override));
    MOCK_METHOD(bool, writeRegister, (const Registers, const Reg), (override));

    MOCK_METHOD(uint32_t, getProgramCounter, (), (const, override));
    MOCK_METHOD(void, setProgramCounter, (uint32_t), (override));

    MOCK_METHOD(bool, programRead, (Reg&), (const, override));

    MOCK_METHOD(bool, pushStack, (const Reg&), (override));
    MOCK_METHOD(bool, popStack, (Reg&), (override));
};

TEST(ScriptTests_Runtime_Instructions, callNopValid)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    auto r = opNop(ctx, 0x00);
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callNopInvalid)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    for (int32_t i = 1; i < 256; ++i)
    {
        auto r = opNop(ctx, (uint8_t)i);
        ASSERT_EQ(r.opSize, 2U);
        ASSERT_EQ(r.ec, ErrorCode::INVALID_INSTRUCTION);
    }

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callAdd_SrcA_DstA)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    Reg reg;
    reg.uVal = 44U;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, readRegister(Registers::AReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{22U}),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, readRegister(Registers::AReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ 22U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::AReg, RegIntMatcher(reg))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = false;

    auto r = opAdd(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callAdd_SrcB_DstA)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    Reg reg;
    reg.uVal = 44U;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, readRegister(Registers::AReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ 22U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, readRegister(Registers::BReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ 22U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::AReg, RegIntMatcher(reg))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::BReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = false;

    auto r = opAdd(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callAdd_SrcInvalid_DstA)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    Reg reg;
    reg.uVal = 44U;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, readRegister(Registers::AReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ 22U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, readRegister(Registers::REG_END, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ 22U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::AReg, RegIntMatcher(reg))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::REG_END;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = true;

    auto r = opAdd(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callMov_SrcA_DstB)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, readRegister(Registers::AReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ 22U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::BReg, RegIntMatcher(Reg{ 22U}))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::BReg;
    flags.mFloating = false;
    flags.mSrcAsValue = false;

    auto r = opMov(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callMov_SrcValue_DstC)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, programRead(::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<0>(Reg{ 22U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::CReg, RegIntMatcher(Reg{ 22U }))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::CReg;
    flags.mFloating = false;
    flags.mSrcAsValue = true;

    auto r = opMov(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 6U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callInc_DstA_Int)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, readRegister(Registers::AReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ 22U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::AReg, RegIntMatcher(Reg{ 23U }))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = false;

    auto r = opInc(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callInc_DstB_Float)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    Reg rd;
    rd.fVal = 55.0F;

    Reg exp;
    exp.fVal = 56.0F;

    EXPECT_CALL(ctx, readRegister(Registers::BReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(rd),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::BReg, RegFloatMatcher(exp))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::BReg;
    flags.mFloating = true;
    flags.mSrcAsValue = false;

    auto r = opInc(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callDec_DstA_Int)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, readRegister(Registers::AReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ 22U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::AReg, RegIntMatcher(Reg{ 21U }))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = false;

    auto r = opDec(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callDec_DstB_Float)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    Reg rd;
    rd.fVal = 55.0F;

    Reg exp;
    exp.fVal = 54.0F;

    EXPECT_CALL(ctx, readRegister(Registers::BReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(rd),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::BReg, RegFloatMatcher(exp))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::BReg;
    flags.mFloating = true;
    flags.mSrcAsValue = false;

    auto r = opDec(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);

    ::testing::Mock::VerifyAndClearExpectations(&ctx);
}

TEST(ScriptTests_Runtime_Instructions, callPush_Reg)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, readRegister(Registers::AReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{55U}),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, pushStack(RegIntMatcher(Reg{ 55U }))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = false;

    auto r = opPush(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);
}

TEST(ScriptTests_Runtime_Instructions, callPush_Value)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, programRead(::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<0>(Reg{ 55U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, pushStack(RegIntMatcher(Reg{ 55U }))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = true;

    auto r = opPush(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 6U);
    ASSERT_EQ(r.ec, ErrorCode::OK);
}

TEST(ScriptTests_Runtime_Instructions, callPop_Reg)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, popStack(::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<0>(Reg{ 55U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, writeRegister(Registers::DReg, RegIntMatcher(Reg{ 55U }))).WillOnce(::testing::Return(true));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::DReg;
    flags.mFloating = false;
    flags.mSrcAsValue = false;

    auto r = opPop(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 2U);
    ASSERT_EQ(r.ec, ErrorCode::OK);
}

TEST(ScriptTests_Runtime_Instructions, callJump_Value)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, programRead(::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<0>(Reg{ 15U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, getProgramCounter()).WillOnce(::testing::Return(10U));
    EXPECT_CALL(ctx, setProgramCounter(25U));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = true;

    auto r = opJmp(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 0U);
    ASSERT_EQ(r.ec, ErrorCode::OK);
}

TEST(ScriptTests_Runtime_Instructions, callJump_ValueBack)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, programRead(::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<0>(Reg{ static_cast<uint32_t>(-5) }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, getProgramCounter()).WillOnce(::testing::Return(10U));
    EXPECT_CALL(ctx, setProgramCounter(5U));

    OpFlags flags;
    flags.mSrcReg = Registers::AReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = true;

    auto r = opJmp(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 0U);
    ASSERT_EQ(r.ec, ErrorCode::OK);
}


TEST(ScriptTests_Runtime_Instructions, callJump_Reg)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, readRegister(Registers::CReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ 15U }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, getProgramCounter()).WillOnce(::testing::Return(10U));
    EXPECT_CALL(ctx, setProgramCounter(25U));

    OpFlags flags;
    flags.mSrcReg = Registers::CReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = false;

    auto r = opJmp(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 0U);
    ASSERT_EQ(r.ec, ErrorCode::OK);
}

TEST(ScriptTests_Runtime_Instructions, callJump_RegBack)
{
    ::testing::StrictMock<ProgramContextMock> ctx;

    ::testing::InSequence s;

    EXPECT_CALL(ctx, readRegister(Registers::CReg, ::testing::_)).WillOnce(
        ::testing::DoAll(
            ::testing::SetArgReferee<1>(Reg{ static_cast<uint32_t>(-5) }),
            ::testing::Return(true)
        )
    );

    EXPECT_CALL(ctx, getProgramCounter()).WillOnce(::testing::Return(10U));
    EXPECT_CALL(ctx, setProgramCounter(5U));

    OpFlags flags;
    flags.mSrcReg = Registers::CReg;
    flags.mDstReg = Registers::AReg;
    flags.mFloating = false;
    flags.mSrcAsValue = false;

    auto r = opJmp(ctx, opFlagsToFlags(flags));
    ASSERT_EQ(r.opSize, 0U);
    ASSERT_EQ(r.ec, ErrorCode::OK);
}
