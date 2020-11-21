
#pragma once

#include <cstdint>

enum class Registers : uint8_t
{
    AReg = 0x00,
    BReg = 0x01,
    CReg = 0x02,
    DReg = 0x03,
    FpReg = 0x04,
    BpReg = 0x05,
    XReg = 0x06,
    YReg = 0x07,
    REG_END = 0x08,
};

union Reg
{
    uint32_t uVal;
    float fVal;
};

struct OpFlags
{
    bool mFloating;
    bool mSrcAsValue;
    Registers mDstReg;
    Registers mSrcReg;
};

OpFlags flagsToOpFlags(uint8_t flags);
uint8_t opFlagsToFlags(OpFlags);
