#ifndef X11_INPUT_HPP
#define X11_INPUT_HPP

#ifdef SW_LINUX_XLIB

#include <common/Common.hpp>
#include <swizzle/core/Input.hpp>

namespace swizzle::core
{
    void InputInit();
    void InputCleanup();

    input::Keys ScanCode2Key(S32 scanCode);

    S32 Key2ScanCode(input::Keys key);

    const SwChar* GetWinKeyText(S32 scanCode);
    const SwWChar* GetWinKeyTextW(S32 scanCode);
} // namespace swizzle::core

#endif

#endif