#ifndef WIN32_XInput_HPP
#define WIN32_XInput_HPP

#ifdef SW_WINDOWS

/* Include files */

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>

#include "Win32Window.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace win32
{
    void ProcessXInput(swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler);
}

#endif

#endif
