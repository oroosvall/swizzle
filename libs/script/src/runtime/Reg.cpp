
#include "runtime/Reg.hpp"

OpFlags flagsToOpFlags(uint8_t flags)
{
    OpFlags fl;

    fl.mFloating = (flags & 0x80U) == 0x80U;
    fl.mDstReg = static_cast<Registers>((flags & 0x70U) >> 4U);
    fl.mSrcReg = static_cast<Registers>(flags & 0x07U);
    fl.mSrcAsValue = (flags & 0x08U) == 0x08U;
    if (fl.mSrcAsValue)
    {
        fl.mSrcReg = Registers::REG_END;
    }

    return fl;
}

uint8_t opFlagsToFlags(OpFlags fl)
{
    uint8_t flags = 0x0U;
    if (fl.mFloating)
    {
        flags |= 0x80U;
    }
    flags |= ((static_cast<uint8_t>(fl.mDstReg) << 4) & 0x70U);
    flags |= (static_cast<uint8_t>(fl.mSrcReg) & 0x07U);
    if (fl.mSrcAsValue)
    {
        flags |= 0x0FU;
    }

    return flags;
}