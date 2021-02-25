#pragma once

#ifdef SW_WINDOWS

#include <common/Types.hpp>
#include <swizzle/core/Input.hpp>

namespace swizzle::core
{
    void InputInit();
    void InputCleanup();

    const input::Keys ScanCode2Key(S32 scanCode);

    S32 Key2ScanCode(input::Keys key);

    const SwChar* GetWinKeyText(S32 scanCode);
    const SwWChar* GetWinKeyTextW(S32 scanCode);
}

#endif