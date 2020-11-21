
#pragma once

#include <cstdint>
#include "ErrorCode.hpp"

class ProgramContext;

enum class Ops : uint8_t
{
    NOP = 0x00,
    ADD = 0x01,
    PUSH = 0x02,
    POP = 0x03,
    MOV = 0x04,
    INC = 0x05,
    DEC = 0x06,
    SUB = 0x07,
    MUL = 0x08,
    DIV = 0x09
};

struct OpReturn
{
    uint8_t opSize;
    ErrorCode ec;
};

// returns size of instruction
typedef OpReturn(*OpFunc)(ProgramContext& ctx, uint8_t flags);

struct OpCode
{
    Ops op1;
    OpFunc operatorFunction;
};
