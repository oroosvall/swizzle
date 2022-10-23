
#ifdef SW_WINDOWS

/* Include files */

#include "RawInput.hpp"

#include <swizzle/core/Logging.hpp>

/* Defines */

#define RAW_INPUT_CASE(caseName, key, down)                                                                            \
    case caseName: {                                                                                                   \
        swizzle::core::InputEvent evt;                                                                                 \
        evt.mFromKeyboard = false;                                                                                     \
        evt.mKey = key;                                                                                                \
        evt.mModKeys = window->modKeys;                                                                                \
        evt.mPressed = down;                                                                                           \
        evtHandler.publishEvent(evt);                                                                                  \
        break;                                                                                                         \
    }

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

namespace win32
{
    static void mouseMoveEvent(swizzle::core::Win32Window* window,
                               swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler);
    static void mouseMoveDeltaEvent(swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler,
                                    RAWMOUSE& mouseEvents);
    static void mouseButtonEvent(swizzle::core::Win32Window* window,
                                 swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler,
                                 RAWMOUSE& mouseEvents);
    static void mouseScrollEvent(swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler,
                                 RAWMOUSE& mouseEvents);

    static void keyboardModifierKeys(swizzle::core::Win32Window* window, RAWKEYBOARD& keyboardEvents);
    static S32 correctScanCode(U32 scanCode, U32 vKey, U16 flags);
} // namespace win32

/* Static Variables */

/* Static Function Definition */

namespace win32
{
    static void mouseMoveEvent(swizzle::core::Win32Window* window,
                               swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler)
    {
        POINT pt;
        GetCursorPos(&pt);
        if (ScreenToClient((HWND)window->getNativeWindowHandle(), &pt))
        {
            swizzle::core::MouseMoveEvent moveEvt;
            moveEvt.mX = pt.x;
            moveEvt.mY = pt.y;

            moveEvt.mX = std::max(moveEvt.mX, 0);
            moveEvt.mY = std::max(moveEvt.mY, 0);

            evtHandler.publishEvent(moveEvt);
        }
    }

    static void mouseMoveDeltaEvent(swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler,
                                    RAWMOUSE& mouseEvents)
    {
        swizzle::core::MouseMoveDelta moveEvt;
        moveEvt.dX = mouseEvents.lLastX;
        moveEvt.dY = mouseEvents.lLastY;

        evtHandler.publishEvent(moveEvt);
    }

    static void mouseButtonEvent(swizzle::core::Win32Window* window,
                                 swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler,
                                 RAWMOUSE& mouseEvents)
    {
        switch (mouseEvents.usButtonFlags)
        {
            RAW_INPUT_CASE(RI_MOUSE_BUTTON_1_DOWN, 1, true);
            RAW_INPUT_CASE(RI_MOUSE_BUTTON_1_UP, 1, false);

            RAW_INPUT_CASE(RI_MOUSE_BUTTON_2_DOWN, 2, true);
            RAW_INPUT_CASE(RI_MOUSE_BUTTON_2_UP, 2, false);

            RAW_INPUT_CASE(RI_MOUSE_BUTTON_3_DOWN, 3, true);
            RAW_INPUT_CASE(RI_MOUSE_BUTTON_3_UP, 3, false);

            RAW_INPUT_CASE(RI_MOUSE_BUTTON_4_DOWN, 4, true);
            RAW_INPUT_CASE(RI_MOUSE_BUTTON_4_UP, 4, false);

            RAW_INPUT_CASE(RI_MOUSE_BUTTON_5_DOWN, 5, true);
            RAW_INPUT_CASE(RI_MOUSE_BUTTON_5_UP, 5, false);
        default:
            break;
        }
    }

    static void mouseScrollEvent(swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler,
                                 RAWMOUSE& mouseEvents)
    {
        short scrollX = 0;
        short scrollY = 0;

        if (RI_MOUSE_WHEEL == mouseEvents.usButtonFlags)
        {
            scrollY = (short)mouseEvents.usButtonData / WHEEL_DELTA;
        }
        if (RI_MOUSE_HWHEEL == mouseEvents.usButtonFlags)
        {
            scrollX = (short)mouseEvents.usButtonData / WHEEL_DELTA;
        }

        swizzle::core::MouseScrollEvent evt;
        evt.mScrollX = scrollX;
        evt.mScrollY = scrollY;
        evtHandler.publishEvent(evt);
    }

    static void keyboardModifierKeys(swizzle::core::Win32Window* window, RAWKEYBOARD& keyboardEvents)
    {
        if (keyboardEvents.Flags == RI_KEY_MAKE)
        {
            if (keyboardEvents.VKey == VK_SHIFT)
            {
                window->modKeys |= MODKEY_SHIFT;
            }
            else if (keyboardEvents.VKey == VK_CONTROL)
            {
                window->modKeys |= MODKEY_CONTROL;
            }
            else if (keyboardEvents.VKey == VK_MENU)
            {
                window->modKeys |= MODKEY_ALT;
            }
            else if (keyboardEvents.VKey == VK_LWIN || keyboardEvents.VKey == VK_RWIN)
            {
                window->modKeys |= MODKEY_SUPER;
            }
        }
        else
        {
            if (keyboardEvents.VKey == VK_SHIFT)
            {
                window->modKeys ^= MODKEY_SHIFT;
            }
            else if (keyboardEvents.VKey == VK_CONTROL)
            {
                window->modKeys ^= MODKEY_CONTROL;
            }
            else if (keyboardEvents.VKey == VK_MENU)
            {
                window->modKeys ^= MODKEY_ALT;
            }
            else if (keyboardEvents.VKey == VK_LWIN || keyboardEvents.VKey == VK_RWIN)
            {
                window->modKeys ^= MODKEY_SUPER;
            }
        }
    }

    static S32 correctScanCode(U32 scanCode, U32 vKey, U16 flags)
    {
        if (vKey == 255u)
        {
            return 0u;
        }
        else if (vKey == VK_SHIFT)
        {
            // correct left-hand / right-hand SHIFT
            vKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
        }
        else if (vKey == VK_NUMLOCK)
        {
            // correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
            scanCode = (MapVirtualKey(vKey, MAPVK_VK_TO_VSC) | 0x100u);
        }

        bool isE0 = ((flags & RI_KEY_E0) != 0u);
        bool isE1 = ((flags & RI_KEY_E1) != 0u);

        if (isE1)
        {
            // manually map VK_PAUSE, since it is bugged #feature
            if (vKey == VK_PAUSE)
            {
                scanCode = 0x45u;
            }
            else
            {
                scanCode = MapVirtualKey(vKey, MAPVK_VK_TO_VSC);
            }
        }

        scanCode = (scanCode | (isE0 << 8u));

        return (S32)scanCode;
    }

} // namespace win32

/* Function Definition */

namespace win32
{
    void SetupRID(HWND window)
    {
        RAWINPUTDEVICE rid[3]{};

        rid[0].usUsagePage = 0x01;
        rid[0].usUsage = 0x02;
        rid[0].dwFlags = RIDEV_INPUTSINK;
        rid[0].hwndTarget = window;

        rid[1].usUsagePage = 0x01;
        rid[1].usUsage = 0x06;
        rid[1].dwFlags = RIDEV_NOHOTKEYS;
        rid[1].hwndTarget = window;

        rid[2].usUsagePage = 0x01;
        rid[2].usUsage = 0x05;
        rid[2].dwFlags = 0;
        rid[2].hwndTarget = window;

        if (RegisterRawInputDevices(rid, 2u, sizeof(rid[0])) == FALSE)
        {

            LOG_ERROR("RegisterRawInputDevices failed!\n\t %d", GetLastError());
        }
    }

    void ProcessRawMouseEvents(swizzle::core::Win32Window* window,
                               swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler, RAWMOUSE& mouseEvents)
    {
        mouseMoveEvent(window, evtHandler);
        mouseMoveDeltaEvent(evtHandler, mouseEvents);
        mouseButtonEvent(window, evtHandler, mouseEvents);
        mouseScrollEvent(evtHandler, mouseEvents);
    }

    void processRawKeyboardEvents(swizzle::core::Win32Window* window,
                                  swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler,
                                  RAWKEYBOARD& keyboardEvents)
    {
        keyboardModifierKeys(window, keyboardEvents);

        SwBool pressed = keyboardEvents.Message == WM_KEYDOWN;

        swizzle::core::InputEvent in;
        in.mFromKeyboard = true;
        in.mKey = correctScanCode(keyboardEvents.MakeCode, keyboardEvents.VKey, keyboardEvents.Flags);
        in.mPressed = pressed;
        in.mModKeys = window->modKeys;

        if (in.mKey != 0u)
        {
            evtHandler.publishEvent(in);
        }
    }

} // namespace win32

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */

#endif