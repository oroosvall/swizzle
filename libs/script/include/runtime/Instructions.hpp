
#pragma once

#include "OpCodes.hpp"

OpReturn opNop(ProgramContext& ctx, uint8_t flags);
OpReturn opAdd(ProgramContext& ctx, uint8_t flags);
OpReturn opPush(ProgramContext& ctx, uint8_t flags);
OpReturn opPop(ProgramContext& ctx, uint8_t flags);
OpReturn opMov(ProgramContext& ctx, uint8_t flags);
OpReturn opInc(ProgramContext& ctx, uint8_t flags);
OpReturn opDec(ProgramContext& ctx, uint8_t flags);
OpReturn opJmp(ProgramContext& ctx, uint8_t flags);
OpReturn opCmp(ProgramContext& ctx, uint8_t flags);
