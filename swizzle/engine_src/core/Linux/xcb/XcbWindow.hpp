#ifndef XCB_WINDOW_HPP
#define XCB_WINDOW_HPP

#ifdef SW_LINUX_XCB

#include <common/Common.hpp>
#include <swizzle/core/WindowEvents.hpp>
#include <swizzle/core/Window.hpp>

#include "core/EventList.hpp"

#include <xcb/xcb.h>

using SwWindow = swizzle::core::SwWindow;
template<typename T>
using EventHandler = swizzle::EventHandler<T>;
template<typename T>
using EventHandlerList = swizzle::EventHandlerList<T>;
using WindowEvent = swizzle::core::WindowEvent;

namespace xcb
{

    class XcbWindow : public SwWindow
    {
    public:
        XcbWindow(const U32 width, const U32 height, const char* title);
        virtual ~XcbWindow();

        virtual void show() override;
        virtual void hide() override;

        virtual void addEventListener(EventHandler<WindowEvent>* listener) override;
        virtual void removeEventListener(EventHandler<WindowEvent>* listener) override;

        virtual void setTitle(const char* title) override;
        virtual void setTitle(const wchar_t* title) override;

        virtual void setSize(const U32 width, const U32 height) override;

        virtual void setWindowPos(const S32 xPos, const S32 yPos) override;
        virtual void getWindowPos(S32& xPos, S32& yPos) override;

        virtual bool isVisible() const override;
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

        EventHandlerList<WindowEvent>& getEventHandler();

        S32 modKeys;

    private:
        xcb_connection_t* mDisplayConnection;
        xcb_screen_t* mScreen;
        xcb_drawable_t mWindow;

        bool mCursorVisible;

        EventHandlerList<WindowEvent> mEventHandlers;
    };

} // namespace swizzle

#endif

#endif