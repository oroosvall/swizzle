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

namespace swizzle::core
{

    const LPWSTR gWindowClass = L"SwizzleWindowClass";
    LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class Win32Window : public SwWindow
    {
    public:
        Win32Window(const U32 width, const U32 height, const char* title);
        virtual ~Win32Window();

        virtual void show() override;
        virtual void hide() override;

        virtual void addEventListener(EventHandler<WindowEvent>* listener) override;
        virtual void removeEventListener(EventHandler<WindowEvent>* listener) override;

        virtual void setTitle(const char* title) override;
        virtual void setTitle(const wchar_t* title) override;

        virtual void setSize(const U32 width, const U32 height) override;

        virtual void setWindowPos(const U32 xPos, const U32 yPos) override;
        virtual void getWindowPos(U32& xPos, U32& yPos) override;

        virtual bool isVisible() const override;

        virtual bool hasFocus() const override;

        virtual void setBorderless(bool borderless) override;
        virtual void setFullscreen(bool fullscreen) override;

        virtual void getCursorPos(U32& xPos, U32& yPos) const override;
        virtual void getSize(U32& width, U32& height) const override;
        virtual void* getNativeWindowHandle() const override;
        virtual void* getNativeDisplayHandle() const override;
        virtual void pollEvents() override;

        virtual void setCursorVisible(bool visible) override;
        virtual bool isCursorVisible() const override;

        EventHandlerList<WindowEvent>& getEventHandler();

        S32 modKeys;

    private:
        void changeScreenMode();

        HWND mWnd;
        bool mCursorVisible;

        SwBool mFullscreen;
        SwBool mBorderless;
        SwBool mWindowPlacementSet;

        EventHandlerList<WindowEvent> mEventHandlers;

        WINDOWPLACEMENT mWindowPlacement;
    };

} // namespace swizzle::core

#endif

#endif