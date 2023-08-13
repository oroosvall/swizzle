#ifndef RAW_INPUT_HPP
#define RAW_INPUT_HPP

#ifdef SW_WINDOWS

/* Include files */

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include "Win32Window.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace win32
{
    void SetupRID(HWND window);
    void ProcessRawMouseEvents(Win32Window* window, swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler,
                               RAWMOUSE& mouseEvents);
    void ProcessRawKeyboardEvents(Win32Window* window,
                                  swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler,
                                  RAWKEYBOARD& keyboardEvents);

} // namespace win32

#endif

#endif
