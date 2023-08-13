#ifndef WIN32WINDOW_HPP
#define WIN32WINDOW_HPP

#ifdef SW_WINDOWS

#include <common/Common.hpp>
#include <swizzle/core/Window.hpp>
#include <swizzle/core/WindowEvents.hpp>

#include "core/EventList.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace win32
{

    const LPWSTR gWindowClass = (const LPWSTR)L"SwizzleWindowClass";
    LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class Win32Window : public swizzle::core::SwWindow
    {
    public:
        Win32Window(const U32 width, const U32 height, const char* title);
        virtual ~Win32Window();

        virtual void show() override;
        virtual void hide() override;

        virtual void addEventListener(swizzle::EventHandler<swizzle::core::WindowEvent>* listener) override;
        virtual void removeEventListener(swizzle::EventHandler<swizzle::core::WindowEvent>* listener) override;

        virtual void setTitle(const char* title) override;
        virtual void setTitle(const wchar_t* title) override;

        virtual void setSize(const U32 width, const U32 height) override;

        virtual void setWindowPos(const S32 xPos, const S32 yPos) override;
        virtual void getWindowPos(S32& xPos, S32& yPos) override;

        virtual bool isVisible() const override;
        virtual bool isMinimized() const override;

        virtual bool hasFocus() const override;

        virtual void setBorderless(bool borderless) override;
        virtual void setFullscreen(bool fullscreen) override;

        virtual void setCursorPos(const S32 xPos, const S32 yPos) override;
        virtual void getCursorPos(S32& xPos, S32& yPos) const override;
        virtual void getSize(U32& width, U32& height) const override;
        virtual void* getNativeWindowHandle() const override;
        virtual void* getNativeDisplayHandle() const override;
        virtual void pollEvents();

        virtual void setCursorVisible(bool visible) override;
        virtual bool isCursorVisible() const override;

        swizzle::EventHandlerList<swizzle::core::WindowEvent>& getEventHandler();

        S32 modKeys;

        bool mTrackCursor;

    private:
        void changeScreenMode();

        HWND mWnd;
        bool mCursorVisible;

        SwBool mFullscreen;
        SwBool mBorderless;
        SwBool mWindowPlacementSet;

        swizzle::EventHandlerList<swizzle::core::WindowEvent> mEventHandlers;

        WINDOWPLACEMENT mWindowPlacement;
    };

} // namespace swizzle::core

#endif

#endif