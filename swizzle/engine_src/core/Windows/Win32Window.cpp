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

#include <algorithm>

namespace plf::window
{
    SwBool init()
    {
        return true;
    }

    void cleanup()
    {
    }

    void pollWindowEvents()
    {
        MSG msg;

        while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

namespace swizzle::core
{
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
            win32::processRawKeyboardEvents(window, evtHandler, raw->data.keyboard);
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            win32::ProcessRawMouseEvents(window, evtHandler, raw->data.mouse);
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
                    WindowFocusEvent evt;
                    evt.mWindow = wnd;
                    evt.mFocused = true;
                    eventHandler.publishEvent(evt);
                    XInputEnable(true);
                }
                else
                {
                    WindowFocusEvent evt;
                    evt.mWindow = wnd;
                    evt.mFocused = false;
                    eventHandler.publishEvent(evt);
                    XInputEnable(false);
                    break;
                }
                break;
            }
            case WM_SETFOCUS: {
                WindowFocusEvent evt;
                evt.mWindow = wnd;
                evt.mFocused = true;
                eventHandler.publishEvent(evt);
                XInputEnable(true);
                win32::SetupRID(hWnd);
                break;
            }
            case WM_KILLFOCUS: {
                WindowFocusEvent evt;
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
            case WM_MOVE:
            {
                int xPos = (int)(short)LOWORD(lParam);   // horizontal position
                int yPos = (int)(short)HIWORD(lParam);   // vertical position

                WindowMoveEvent evt;
                evt.mWindow = wnd;
                evt.mXPos = xPos;
                evt.mYPos = yPos;
                eventHandler.publishEvent(evt);
                break;
            }
            case WM_SIZE: {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);

                WindowResizeEvent evt;
                evt.mWindow = wnd;
                evt.mHeight = height;
                evt.mWidth = width;
                eventHandler.publishEvent(evt);
                break;
            }
            case WM_CLOSE: {
                WindowCloseEvent evt;
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

                CharacterEvent evt;
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
            case WM_MOUSEMOVE: {

                if(!wnd->mTrackCursor)
                {
                    TRACKMOUSEEVENT tme;
                    ZeroMemory(&tme, sizeof(tme));
                    tme.cbSize = sizeof(tme);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hWnd;
                    TrackMouseEvent(&tme);
                    wnd->mTrackCursor = true;

                    MouseEnterEvent evt{};
                    evt.mWindow = wnd;
                    evt.mEnter = true;
                    eventHandler.publishEvent(evt);
                }

                return 0;
            }
            case WM_MOUSELEAVE: {
                MouseEnterEvent evt{};
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

    void Win32Window::setWindowPos(const S32 xPos, const S32 yPos)
    {
        RECT r = {xPos, yPos, xPos, yPos};
        AdjustWindowRectEx(&r, GetWindowLongPtr(mWnd, GWL_STYLE),
                            FALSE, GetWindowLongPtr(mWnd, GWL_EXSTYLE));
        SetWindowPos(mWnd, NULL, xPos, yPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    void Win32Window::getWindowPos(S32& xPos, S32& yPos)
    {
        POINT p{};
        ClientToScreen(mWnd, &)
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
        SWIZZLE_PROFILE_EVENT();
        MSG msg;

        win32::ProcessXInput(this, mEventHandlers);

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