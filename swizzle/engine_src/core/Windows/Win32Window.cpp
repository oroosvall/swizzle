#ifdef SW_WINDOWS

#include <swizzle/core/Logging.hpp>
#include <swizzle/core/WindowEvents.hpp>
#include <swizzle/profiler/Profiler.hpp>

#include "RawInput.hpp"
#include "Win32Window.hpp"
#include "Win32Xinput.hpp"

#include <string>

#pragma warning(disable : 4995)
#include <Xinput.h>
#include <windowsx.h>

#include <algorithm>

namespace plf::window
{
    SwBool init()
    {
        return true;
    }

    void cleanup() {}

    void pollWindowEvents()
    {
        MSG msg;

        while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
} // namespace plf::window

namespace win32
{
    void inputCallback(Win32Window* window, swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler, LPARAM lParam)
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
            ProcessRawKeyboardEvents(window, evtHandler, raw->data.keyboard);
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            ProcessRawMouseEvents(window, evtHandler, raw->data.mouse);
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
            case WM_ACTIVATE: {
                if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
                {
                    swizzle::core::WindowFocusEvent evt;
                    evt.mWindow = wnd;
                    evt.mFocused = true;
                    // eventHandler.publishEvent(evt);
                    XInputEnable(true);
                }
                else
                {
                    swizzle::core::WindowFocusEvent evt;
                    evt.mWindow = wnd;
                    evt.mFocused = false;
                    // eventHandler.publishEvent(evt);
                    XInputEnable(false);
                    break;
                }
                break;
            }
            case WM_SETFOCUS: {
                swizzle::core::WindowFocusEvent evt;
                evt.mWindow = wnd;
                evt.mFocused = true;
                eventHandler.publishEvent(evt);
                XInputEnable(true);
                win32::SetupRID(hWnd);
                break;
            }
            case WM_KILLFOCUS: {
                swizzle::core::WindowFocusEvent evt;
                evt.mWindow = wnd;
                evt.mFocused = false;
                eventHandler.publishEvent(evt);
                XInputEnable(false);
                break;
            }
            case WM_SYSCOMMAND: {
                switch (wParam)
                {
                case SC_KEYMENU:
                    return 0;
                }
                break;
            }
            case WM_SETCURSOR: {
                if (!wnd->isCursorVisible())
                {
                    if (HTCLIENT == LOWORD(lParam))
                    {
                        SetCursor(NULL);
                    }
                }
                break;
            }
            case WM_MOVE: {
                int xPos = (int)(short)LOWORD(lParam); // horizontal position
                int yPos = (int)(short)HIWORD(lParam); // vertical position

                swizzle::core::WindowMoveEvent evt;
                evt.mWindow = wnd;
                evt.mXPos = xPos;
                evt.mYPos = yPos;
                eventHandler.publishEvent(evt);
                break;
            }
            case WM_SIZE: {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);

                swizzle::core::WindowResizeEvent evt;
                evt.mWindow = wnd;
                evt.mHeight = height;
                evt.mWidth = width;
                eventHandler.publishEvent(evt);
                break;
            }
            case WM_CLOSE: {
                swizzle::core::WindowCloseEvent evt;
                evt.mWindow = wnd;
                eventHandler.publishEvent(evt);
                break;
            }
            case WM_CHAR:
            case WM_SYSCHAR:
            case WM_UNICHAR: {
                // const bool plain = (msg != WM_SYSCHAR);

                if (msg == WM_UNICHAR && wParam == UNICODE_NOCHAR)
                {
                    return TRUE;
                }

                const unsigned int codePoint = (unsigned int)wParam;
                if (codePoint < 32 || (codePoint > 126 && codePoint < 160))
                {
                    return 0;
                }

                swizzle::core::CharacterEvent evt;
                evt.mWindow = wnd;
                evt.mCodePoint = codePoint;
                if (wnd->hasFocus())
                {
                    eventHandler.publishEvent(evt);
                }

                return 0;
                break;
            }
            case WM_INPUT: {
                if (wnd->hasFocus())
                {
                    inputCallback(wnd, eventHandler, lParam);
                }
                break;
            }
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_XBUTTONDOWN:
            {
                swizzle::core::InputEvent evt{};
                evt.mWindow = wnd;
                evt.mFromKeyboard = false;
                evt.mPressed = true;
                if (msg == WM_LBUTTONDOWN)
                    evt.mKey = 1;
                else if (msg == WM_MBUTTONDOWN)
                    evt.mKey = 3;
                else if (msg == WM_RBUTTONDOWN)
                    evt.mKey = 2;
                else if (msg == WM_XBUTTONDOWN)
                {
                    if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                        evt.mKey = 4;
                    else
                        evt.mKey = 5;
                }

                eventHandler.publishEvent(evt);
                break;
            }
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            case WM_XBUTTONUP:
            {
                swizzle::core::InputEvent evt{};
                evt.mWindow = wnd;
                evt.mFromKeyboard = false;
                evt.mPressed = false;
                if (msg == WM_LBUTTONUP)
                    evt.mKey = 1;
                else if (msg == WM_MBUTTONUP)
                    evt.mKey = 3;
                else if (msg == WM_RBUTTONUP)
                    evt.mKey = 2;
                else if (msg == WM_XBUTTONUP)
                {
                    if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                        evt.mKey = 4;
                    else
                        evt.mKey = 5;
                }

                eventHandler.publishEvent(evt);
                break;
            }
            case WM_MOUSEMOVE: {

                const int xPos = GET_X_LPARAM(lParam); // horizontal position
                const int yPos = GET_Y_LPARAM(lParam); // vertical position

                if (!wnd->mTrackCursor)
                {
                    TRACKMOUSEEVENT tme;
                    ZeroMemory(&tme, sizeof(tme));
                    tme.cbSize = sizeof(tme);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hWnd;
                    TrackMouseEvent(&tme);
                    wnd->mTrackCursor = true;

                    swizzle::core::MouseEnterEvent evt{};
                    evt.mWindow = wnd;
                    evt.mEnter = true;
                    eventHandler.publishEvent(evt);
                }

                swizzle::core::MouseMoveEvent eMove{};
                eMove.mWindow = wnd;
                eMove.mX = xPos;
                eMove.mY = yPos;
                eventHandler.publishEvent(eMove);

                return 0;
            }
            case WM_MOUSELEAVE: {
                swizzle::core::MouseEnterEvent evt{};
                evt.mWindow = wnd;
                evt.mEnter = false;
                eventHandler.publishEvent(evt);
                wnd->mTrackCursor = false;
                return 0;
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
        , mFullscreen(false)
        , mBorderless(false)
        , mWindowPlacementSet(false)
        , mEventHandlers()
        , modKeys(0)
        , mTrackCursor(false)
        , mWindowPlacement({sizeof(mWindowPlacement)})
    {

        mWnd = CreateWindowEx(WS_EX_APPWINDOW, gWindowClass, L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                              CW_USEDEFAULT, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);

        setTitle(title);
        SetWindowLongPtr(mWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        win32::SetupRID(mWnd);

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

    void Win32Window::addEventListener(swizzle::EventHandler<swizzle::core::WindowEvent>* listener)
    {
        mEventHandlers.addListener(listener);
    }

    void Win32Window::removeEventListener(swizzle::EventHandler<swizzle::core::WindowEvent>* listener)
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
        SetWindowPos(mWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Win32Window::setWindowPos(const S32 xPos, const S32 yPos)
    {
        RECT r = {xPos, yPos, xPos, yPos};
        const DWORD style = static_cast<DWORD>(GetWindowLongPtr(mWnd, GWL_STYLE));
        const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtr(mWnd, GWL_EXSTYLE));
        AdjustWindowRectEx(&r, style, FALSE, exStyle);
        SetWindowPos(mWnd, HWND_TOP, xPos, yPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Win32Window::getWindowPos(S32& xPos, S32& yPos)
    {
        POINT p{};
        ClientToScreen(mWnd, &p);
        xPos = p.x;
        yPos = p.y;
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

    bool Win32Window::isMinimized() const
    {
        return IsIconic(mWnd);
    }

    bool Win32Window::hasFocus() const
    {
        return (mWnd == GetFocus());
    }

    void Win32Window::setBorderless(bool borderless)
    {
        mBorderless = borderless;
        changeScreenMode();
    }

    void Win32Window::setFullscreen(bool fullscreen)
    {
        mFullscreen = fullscreen;
        changeScreenMode();
    }

    void Win32Window::setCursorPos(const S32 xPos, const S32 yPos)
    {
        POINT p = {xPos, yPos};

        ClientToScreen(mWnd, &p);
        SetCursorPos(p.x, p.y);
    }

    void Win32Window::getCursorPos(S32& xPos, S32& yPos) const
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
        GetClientRect(mWnd, &rect);

        width = rect.right;
        height = rect.bottom;
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
        SWIZZLE_PROFILE_EVENT();
        MSG msg;

        ProcessXInput(this, mEventHandlers);

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

    swizzle::EventHandlerList<swizzle::core::WindowEvent>& Win32Window::getEventHandler()
    {
        return mEventHandlers;
    }

    void Win32Window::changeScreenMode()
    {
        DWORD style = GetWindowLong(mWnd, GWL_STYLE);
        if (mBorderless || mFullscreen)
        {
            SetWindowLong(mWnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
        }
        else
        {
            SetWindowLong(mWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        }

        if (mFullscreen)
        {
            MONITORINFO mi = {sizeof(mi)};
            if (GetWindowPlacement(mWnd, &mWindowPlacement) &&
                GetMonitorInfo(MonitorFromWindow(mWnd, MONITOR_DEFAULTTOPRIMARY), &mi))
            {
                SetWindowPos(mWnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                             mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
                             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
                mWindowPlacementSet = true;
            }
        }
        else
        {
            if (mWindowPlacementSet)
            {
                SetWindowPlacement(mWnd, &mWindowPlacement);
                SetWindowPos(mWnd, NULL, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
        }
    }

} // namespace swizzle::core

#endif