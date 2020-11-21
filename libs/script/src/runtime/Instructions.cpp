
#include "runtime/Instructions.hpp"
#include "runtime/ProgramContext.hpp"

OpReturn opNop(ProgramContext& ctx, uint8_t flags)
{
    ctx;
    ErrorCode ec = ErrorCode::OK;
    if (flags != 0U)
    {
        ec = ErrorCode::INVALID_INSTRUCTION;
    }
    return { 2U, ec };
}

OpReturn opAdd(ProgramContext& ctx, uint8_t flags)
{
    Reg dstReg;
    Reg srcReg;

    OpFlags extractedFlags = flagsToOpFlags(flags);

    ctx.readRegister(extractedFlags.mDstReg, dstReg);
    ctx.readRegister(extractedFlags.mSrcReg, srcReg);

    if (extractedFlags.mFloating)
    {
        dstReg.fVal += srcReg.fVal;
    }
    else
    {
        dstReg.uVal += srcReg.uVal;
    }

    ctx.writeRegister(extractedFlags.mDstReg, dstReg);
    return { 2U, ErrorCode::OK };
}

OpReturn opPush(ProgramContext& ctx, uint8_t flags)
{
    OpFlags extractedFlags = flagsToOpFlags(flags);
    uint8_t size = 2U;
    ErrorCode ec = ErrorCode::OK;

    Reg r{};
    if (extractedFlags.mSrcAsValue)
    {
        if (!ctx.programRead(r))
        {
            ec = ErrorCode::PROGRAM_READ;
        }
        size += 4U;
    }
    else
    {
        ctx.readRegister(extractedFlags.mSrcReg, r);
    }

    if (flags & 0xF0U)
    {
        ec = ErrorCode::INVALID_INSTRUCTION;
    }

    if (!ctx.pushStack(r))
    {
        ec = ErrorCode::STACK_OVERFLOW;
    }

    return { size, ec };
}

OpReturn opPop(ProgramContext& ctx, uint8_t flags)
{
    OpFlags extractedFlags = flagsToOpFlags(flags);
    ErrorCode ec = ErrorCode::OK;

    Reg r{};

    if (flags & 0x0FU)
    {
        ec = ErrorCode::INVALID_INSTRUCTION;
    }
    else
    {
        if (!ctx.popStack(r))
        {
            ec = ErrorCode::STACK_UNDERFLOW;
        }
    }

    ctx.writeRegister(extractedFlags.mDstReg, r);

    return { 2U, ec };
}

OpReturn opMov(ProgramContext& ctx, uint8_t flags)
{
    uint8_t size = 2U;
    ErrorCode ec = ErrorCode::OK;

    OpFlags extractedFlags = flagsToOpFlags(flags);

    if (extractedFlags.mFloating)
    {
        ec = ErrorCode::INVALID_INSTRUCTION;
    }

    Reg r{};
    if (extractedFlags.mSrcAsValue)
    {
        if (!ctx.programRead(r))
        {
            ec = ErrorCode::PROGRAM_READ;
        }
        size += 4U;
    }
    else
    {
        ctx.readRegister(extractedFlags.mSrcReg, r);
    }

    ctx.writeRegister(extractedFlags.mDstReg, r);

    return { size, ec };
}

OpReturn opInc(ProgramContext& ctx, uint8_t flags)
{
    OpFlags extractedFlags = flagsToOpFlags(flags);

    Reg r{};
    ctx.readRegister(extractedFlags.mDstReg, r);

    if (extractedFlags.mFloating)
    {
        r.fVal += 1.0F;
    }
    else
    {
        r.uVal += 1;
    }

    ctx.writeRegister(extractedFlags.mDstReg, r);
    return { 2U, ErrorCode::OK };
}

OpReturn opDec(ProgramContext& ctx, uint8_t flags)
{
    OpFlags extractedFlags = flagsToOpFlags(flags);

    Reg r{};
    ctx.readRegister(extractedFlags.mDstReg, r);

    if (extractedFlags.mFloating)
    {
        r.fVal -= 1.0F;
    }
    else
    {
        r.uVal -= 1;
    }

    ctx.writeRegister(extractedFlags.mDstReg, r);
    return { 2U, ErrorCode::OK };
}

OpReturn opJmp(ProgramContext& ctx, uint8_t flags)
{
    ErrorCode ec = ErrorCode::OK;

    OpFlags extractedFlags = flagsToOpFlags(flags);

    if ((flags & 0xF0) != 0U)
    {
        ec = ErrorCode::INVALID_INSTRUCTION;
    }
    else
    {
        Reg r{};
        if (extractedFlags.mSrcAsValue)
        {
            if (ctx.programRead(r))
            {
                uint32_t pcount = ctx.getProgramCounter();
                pcount += r.uVal;

                ctx.setProgramCounter(pcount);
            }
            else
            {
                ec = ErrorCode::PROGRAM_READ;
            }
        }
        else
        {
            ctx.readRegister(extractedFlags.mSrcReg, r);

            uint32_t pcount = ctx.getProgramCounter();
            pcount += r.uVal;

            ctx.setProgramCounter(pcount);
        }
    }

    return { 0U, ec };
}

OpReturn opCmp(ProgramContext& ctx, uint8_t flags)
{

    return { 0U, ErrorCode::OK };
}