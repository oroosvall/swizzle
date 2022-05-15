#include <gtest/gtest.h>
#include "runtime/ProgramContext.hpp"

TEST(ScriptTests_Runtime_ProgramContext, writeRegister)
{
    ProgramContext ctx(10U);

    Reg reg;
    reg.uVal = 123U;

    bool ok = ctx.writeRegister(Registers::AReg, reg);
    ASSERT_EQ(ok, true);

    ok = ctx.readRegister(Registers::AReg, reg);
    ASSERT_EQ(ok, true);
    ASSERT_EQ(reg.uVal, 123U);
}

TEST(ScriptTests_Runtime_ProgramContext, writeRegisterOutOfBound)
{
    ProgramContext ctx(10U);

    Reg reg;
    reg.uVal = 123U;

    bool ok = ctx.writeRegister(Registers::REG_END, reg);
    ASSERT_EQ(ok, false);

    ok = ctx.readRegister(Registers::REG_END, reg);
    ASSERT_EQ(ok, false);
    ASSERT_EQ(reg.uVal, 0U);
}

TEST(ScriptTests_Runtime_ProgramContext, writeProgramCounter)
{
    ProgramContext ctx(10U);

    ctx.setProgramCounter(44U);

    uint32_t ctr = ctx.getProgramCounter();
    ASSERT_EQ(ctr, 44U);
}
