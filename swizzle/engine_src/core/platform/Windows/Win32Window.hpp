#ifndef WIN32WINDOW_HPP
#define WIN32WINDOW_HPP

#ifdef SW_WINDOWS

#include <swizzle/core/common/Types.hpp>
#include <swizzle/core/common/WindowEvents.hpp>
#include <swizzle/core/platform/Window.hpp>

#include "core/common/EventList.hpp"

#include <Windows.h>

namespace swizzle::core
{

    const LPWSTR gWindowClass = L"SwizzleWindowClass";
    LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class Win32Window : public Window
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

        virtual bool isVisible() const override;

        virtual void setBorderless(bool borderless) override;

        virtual void getCursorPos(U32& xPos, U32& yPos) const override;
        virtual void getSize(U32& width, U32& height) const override;
        virtual void* getNativeHandle() const override;
        virtual void pollEvents() override;

        virtual void setCursorVisible(bool visible) override;
        virtual bool isCursorVisible() const override;

        EventHandlerList<WindowEvent>& getEventHandler();

        S32 modKeys;

    private:
        HWND mWnd;
        bool mCursorVisible;

        EventHandlerList<WindowEvent> mEventHandlers;
    };

} // namespace swizzle

#endif

#endif