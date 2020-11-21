
#pragma once

#include <cstdint>

#include "Reg.hpp"

class ProgramContext
{
public:

    ProgramContext(uint32_t stackSize);
    virtual ~ProgramContext();

    virtual bool readRegister(const Registers regIndex, Reg &r) const;
    virtual bool writeRegister(const Registers regIndex, const Reg r);

    virtual uint32_t getProgramCounter() const;
    virtual void setProgramCounter(const uint32_t progCounter);

    virtual bool programRead(Reg& reg) const;

    virtual bool pushStack(const Reg& r);
    virtual bool popStack(Reg& r);

    uint32_t mStackSize;
    uint8_t* mStack;

    uint32_t mProgSize;
    uint8_t* mProg;

private:

    Reg mRegs[uint8_t(Registers::REG_END)];
    uint32_t mProgramCounter;

    bool mOverflowFlag;
    bool mZeroFlag;
    bool mCarryFlag;

};

