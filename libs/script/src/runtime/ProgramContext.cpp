
#include "runtime/ProgramContext.hpp"

#include <string>

ProgramContext::ProgramContext(uint32_t stackSize)
    : mStackSize(stackSize)
    , mStack(new uint8_t[stackSize])
    , mProgSize(0U)
    , mProg(nullptr)
    , mProgramCounter(0U)
    , mRegs()
    , mOverflowFlag(false)
    , mZeroFlag(false)
    , mCarryFlag(false)
{
}

ProgramContext::~ProgramContext()
{
    delete[] mStack;
    delete[] mProg;
}

bool ProgramContext::readRegister(const Registers regIndex, Reg& r) const
{
    bool success = false;
    if (regIndex < Registers::REG_END)
    {
        r = mRegs[(uint8_t)regIndex];
        success = true;
    }
    else
    {
        r.uVal = 0U;
    }
    return success;
}

bool ProgramContext::writeRegister(const Registers regIndex, const Reg r)
{
    bool success = false;
    if (regIndex < Registers::REG_END)
    {
        mRegs[(uint8_t)regIndex] = r;
        success = true;
    }
    return success;
}

uint32_t ProgramContext::getProgramCounter() const
{
    return mProgramCounter;
}

void ProgramContext::setProgramCounter(const uint32_t progCounter)
{
    mProgramCounter = progCounter;
}

bool ProgramContext::programRead(Reg& reg) const
{
    bool success = false;
    if (mProgramCounter + sizeof(reg.uVal) < mProgSize)
    {
        memcpy(&reg.uVal, &mProg[mProgramCounter + 2U], sizeof(reg.uVal));
        success = true;
    }
    return success;
}

bool ProgramContext::pushStack(const Reg& r)
{
    r;
    return true;
}

bool ProgramContext::popStack(Reg& r)
{
    r;
    return true;
}