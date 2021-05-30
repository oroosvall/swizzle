#ifdef SW_WINDOWS

#include <swizzle/core/WindowEvents.hpp>
#include <swizzle/core/Logging.hpp>

#include "Win32Window.hpp"

//#include "../../Logging/Logger.hpp"
#include <string>

#pragma warning(disable : 4995)
#include <Xinput.h>

#undef max

#include <algorithm>

namespace swizzle::core
{

    void setupRID(HWND window)
    {
        RAWINPUTDEVICE rid[3];

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

        if (RegisterRawInputDevices(rid, 2, sizeof(rid[0])) == FALSE)
        {

            LOG_ERROR("Register RID failed!\n\t %d", GetLastError());
        }
    }

    void processRawMouseEvents(Win32Window* window, EventHandlerList<WindowEvent>& evtHandler, RAWMOUSE& mouseEvents)
    {

        // Mouse move event
        {
            POINT pt;
            GetCursorPos(&pt);
            if (ScreenToClient((HWND)window->getNativeWindowHandle(), &pt))
            {
                MouseMoveEvent moveEvt;
                moveEvt.mX = pt.x;
                moveEvt.mY = pt.y;

                moveEvt.mX = std::max(moveEvt.mX, 0);
                moveEvt.mY = std::max(moveEvt.mY, 0);

                evtHandler.publishEvent(moveEvt);
            }
        }

        {
            MouseMoveDelta moveEvt;
            moveEvt.dX = mouseEvents.lLastX;
            moveEvt.dY = mouseEvents.lLastY;

            evtHandler.publishEvent(moveEvt);
        }

        // Mouse button event
        {
            switch (mouseEvents.usButtonFlags)
            {
            case RI_MOUSE_BUTTON_1_DOWN: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 1;
                evt.mModKeys = window->modKeys;
                evt.mPressed = true;
                evtHandler.publishEvent(evt);
            }
            break;
            case RI_MOUSE_BUTTON_1_UP: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 1;
                evt.mModKeys = window->modKeys;
                evt.mPressed = false;
                evtHandler.publishEvent(evt);
            }
            break;
            case RI_MOUSE_BUTTON_2_DOWN: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 2;
                evt.mModKeys = window->modKeys;
                evt.mPressed = true;
                evtHandler.publishEvent(evt);
            }
            break;
            case RI_MOUSE_BUTTON_2_UP: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 2;
                evt.mModKeys = window->modKeys;
                evt.mPressed = false;
                evtHandler.publishEvent(evt);
            }
            break;
            case RI_MOUSE_BUTTON_3_DOWN: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 3;
                evt.mModKeys = window->modKeys;
                evt.mPressed = true;
                evtHandler.publishEvent(evt);
            }
            break;
            case RI_MOUSE_BUTTON_3_UP: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 3;
                evt.mModKeys = window->modKeys;
                evt.mPressed = false;
                evtHandler.publishEvent(evt);
            }
            break;
            case RI_MOUSE_BUTTON_4_DOWN: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 4;
                evt.mModKeys = window->modKeys;
                evt.mPressed = true;
                evtHandler.publishEvent(evt);
            }
            break;
            case RI_MOUSE_BUTTON_4_UP: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 4;
                evt.mModKeys = window->modKeys;
                evt.mPressed = false;
                evtHandler.publishEvent(evt);
            }
            break;
            case RI_MOUSE_BUTTON_5_DOWN: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 5;
                evt.mModKeys = window->modKeys;
                evt.mPressed = true;
                evtHandler.publishEvent(evt);
            }
            break;
            case RI_MOUSE_BUTTON_5_UP: {
                InputEvent evt;
                evt.mFromKeyboard = false;
                evt.mKey = 5;
                evt.mModKeys = window->modKeys;
                evt.mPressed = false;
                evtHandler.publishEvent(evt);
            }
            break;
            default:
                break;
            }
        }

        // Scroll event
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

            MouseScrollEvent evt;
            evt.mScrollX = scrollX;
            evt.mScrollY = scrollY;
            evtHandler.publishEvent(evt);
        }
    }

    U32 correctScanCode(U32 scanCode, U32 vkey, U32 flags)
    {
        if (vkey == 255U)
        {
            return 0U;
        }
        else if (vkey == VK_SHIFT)
        {
            // correct left-hand / right-hand SHIFT
            vkey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
        }
        else if (vkey == VK_NUMLOCK)
        {
            // correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
            scanCode = (MapVirtualKey(vkey, MAPVK_VK_TO_VSC) | 0x100U);
        }

        bool isE0 = ((flags & RI_KEY_E0) != 0);
        bool isE1 = ((flags & RI_KEY_E1) != 0);

        if (isE1)
        {
            // manually map VK_PAUSE, since it is bugged #feature
            if (vkey == VK_PAUSE)
            {
                scanCode = 0x45U;
            }
            else
            {
                scanCode = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
            }
        }

        scanCode = (scanCode | (isE0 << 8U));

        return scanCode;
    }

    void processRawKeyboardEvents(Win32Window* window, EventHandlerList<WindowEvent>& evtHandler,
                                  RAWKEYBOARD& keyboardEvents)
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

        bool pressed = keyboardEvents.Message == WM_KEYDOWN;

        InputEvent in;
        in.mFromKeyboard = true;
        in.mKey = correctScanCode(keyboardEvents.MakeCode, keyboardEvents.VKey, keyboardEvents.Flags);
        in.mPressed = pressed;
        in.mModKeys = window->modKeys;

        if (in.mKey != 0U)
        {
            /*UINT key = (in.mKey << 16U);
            char buffer[512] = {};*/
            /*GetKeyNameTextA((LONG)key, buffer, 512);
            LOG_INFO("%d, %s", in.mKey, buffer);*/

            /*HKL hLocale = LoadKeyboardLayoutA("00000409", KLF_NOTELLSHELL);
            HKL hPrevious = ActivateKeyboardLayout(hLocale, KLF_SETFORPROCESS);*/

            //UINT Vkey = MapVirtualKeyA(in.mKey, MAPVK_VSC_TO_VK);
            //UINT mapping = MapVirtualKeyA(Vkey, MAPVK_VK_TO_CHAR);
            //printf("0x%x, %c\n", mapping, mapping);
            //LOG_INFO("0x%x, %c", mapping, mapping);
            /*ActivateKeyboardLayout(hPrevious, KLF_SETFORPROCESS);
            UnloadKeyboardLayout(hLocale);*/

            //ToAscii()
            /*HKL hLocale = LoadKeyboardLayoutA("00000409", KLF_NOTELLSHELL);
            HKL hPrevious = ActivateKeyboardLayout(hLocale, KLF_SETFORPROCESS);*/

            /*GetKeyNameTextA((LONG)key, buffer, 512);
            LOG_INFO("%d, %s", in.mKey, buffer);*/

            /*ActivateKeyboardLayout(hPrevious, KLF_SETFORPROCESS);
            UnloadKeyboardLayout(hLocale);*/

            evtHandler.publishEvent(in);
        }
    }

    void inputCallback(Win32Window* window, EventHandlerList<WindowEvent>& evtHandler, LPARAM lParam)
    {

        UINT dwSize = 0U;

        if (-1 == GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)))
        {
            LOG_ERROR("Input error %d", GetLastError());
        }

        if (0U == dwSize)
        {
            return;
        }

        LPBYTE lpb = new BYTE[dwSize];

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
        {
            LOG_ERROR("GetRawInputData(...) does not return correct size!");
        }

        RAWINPUT* raw = (RAWINPUT*)lpb;

        if (raw->header.dwType == RIM_TYPEKEYBOARD)
        {
            processRawKeyboardEvents(window, evtHandler, raw->data.keyboard);
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            processRawMouseEvents(window, evtHandler, raw->data.mouse);
        }
        else
        {
            // processRawHidEvents(window, evtHandler, raw);
        }

        delete[] lpb;
    }

    LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        Win32Window* wnd = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (nullptr != wnd)
        {
            auto& eventHandler = wnd->getEventHandler();
            switch (msg)
            {
            case WM_ACTIVATE:
            {
                if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
                {
                    WindowFocusEvent evt;
                    evt.mFocused = true;
                    eventHandler.publishEvent(evt);
                    XInputEnable(true);
                }
                else
                {
                    WindowFocusEvent evt;
                    evt.mFocused = false;
                    eventHandler.publishEvent(evt);
                    XInputEnable(false);
                    break;
                }
                break;
            }
            case WM_SETFOCUS:
            {
                WindowFocusEvent evt;
                evt.mFocused = true;
                eventHandler.publishEvent(evt);
                XInputEnable(true);
                break;
            }
            case WM_KILLFOCUS:
            {
                WindowFocusEvent evt;
                evt.mFocused = false;
                eventHandler.publishEvent(evt);
                XInputEnable(false);
                break;
            }
            case WM_SYSCOMMAND:
            {
                switch (wParam)
                {
                case SC_KEYMENU:
                    return 0;
                }
                break;
            }
            case WM_SETCURSOR:
            {
                if (!wnd->isCursorVisible())
                {
                    if (HTCLIENT == LOWORD(lParam))
                    {
                        SetCursor(NULL);
                    }
                }
                break;
            }
            case WM_SIZE:
            {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);

                WindowResizeEvent evt;
                evt.mHeight = height;
                evt.mWidth = width;
                eventHandler.publishEvent(evt);
                break;
            }
            case WM_CLOSE:
            {
                WindowCloseEvent evt;
                eventHandler.publishEvent(evt);
                break;
            }
            case WM_CHAR:
            case WM_SYSCHAR:
            case WM_UNICHAR:
            {
                const bool plain = (msg != WM_SYSCHAR);

                if (msg == WM_UNICHAR && wParam == UNICODE_NOCHAR)
                {
                    return TRUE;
                }

                const unsigned int codePoint = (unsigned int)wParam;
                if (codePoint < 32 || (codePoint > 126 && codePoint < 160))
                {
                    return 0;
                }

                CharacterEvent evt;
                evt.mCodePoint = codePoint;
                eventHandler.publishEvent(evt);

                return 0;
                break;
            }
            case WM_INPUT: {
                inputCallback(wnd, eventHandler, lParam);
                break;
            }
            default:
                break;
            }
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    Win32Window::Win32Window(const U32 width, const U32 height, const char* title)
        : mWnd(NULL)
        , mCursorVisible(true)
        , mEventHandlers()
        , modKeys(0)
    {

        mWnd = CreateWindowEx(0, gWindowClass, L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width,
                              height, NULL, NULL, GetModuleHandle(NULL), NULL);

        setTitle(title);
        SetWindowLongPtr(mWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        setupRID(mWnd);

        ShowWindow(mWnd, SW_HIDE);
    }

    Win32Window::~Win32Window()
    {
        DestroyWindow(mWnd);
    }

    void Win32Window::show()
    {
        ShowWindow(mWnd, SW_SHOWNA);
    }

    void Win32Window::hide()
    {
        ShowWindow(mWnd, SW_HIDE);
    }

    void Win32Window::addEventListener(EventHandler<WindowEvent>* listener)
    {
        mEventHandlers.addListener(listener);
    }

    void Win32Window::removeEventListener(EventHandler<WindowEvent>* listener)
    {
        mEventHandlers.removeListener(listener);
    }

    void Win32Window::setTitle(const char* title)
    {
        SetWindowTextA(mWnd, title);
    }

    void Win32Window::setTitle(const wchar_t* title)
    {
        SetWindowTextW(mWnd, title);
    }

    void Win32Window::setSize(const U32 width, const U32 height)
    {
        SetWindowPos(mWnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
    }

    bool Win32Window::isVisible() const
    {
        LONG_PTR style = GetWindowLongPtr(mWnd, GWL_STYLE);

        bool visible = false;
        if (style & WS_VISIBLE)
        {
            visible = true;
        }

        return visible;
    }

    void Win32Window::setBorderless(bool borderless)
    {
        DWORD style = GetWindowLong(mWnd, GWL_STYLE);
        if (borderless)
        {
            SetWindowLong(mWnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
        }
        else
        {
            SetWindowLong(mWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        }
    }

    void Win32Window::getCursorPos(U32& xPos, U32& yPos) const
    {
        xPos = yPos = 0;

        POINT p;
        if (GetCursorPos(&p))
        {
            if (ScreenToClient(mWnd, &p))
            {
                xPos = p.x;
                yPos = p.y;
            }
        }
    }

    void Win32Window::getSize(U32& width, U32& height) const
    {

        RECT rect;
        GetWindowRect(mWnd, &rect);

        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }

    void* Win32Window::getNativeWindowHandle() const
    {
        return mWnd;
    }

    void* Win32Window::getNativeDisplayHandle() const
    {
        return nullptr;
    }

    void Win32Window::pollEvents()
    {
        MSG msg;

        // processXInput(this);

        while (PeekMessage(&msg, mWnd, NULL, NULL, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void Win32Window::setCursorVisible(bool visible)
    {
        mCursorVisible = visible;
    }

    bool Win32Window::isCursorVisible() const
    {
        return mCursorVisible;
    }

    EventHandlerList<WindowEvent>& Win32Window::getEventHandler()
    {
        return mEventHandlers;
    }

} // namespace swizzle

#endif