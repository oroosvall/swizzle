#ifndef XCB_WINDOW_HPP
#define XCB_WINDOW_HPP

#ifdef SW_LINUX_XCB

#include <common/Common.hpp>
#include <swizzle/core/WindowEvents.hpp>
#include <swizzle/core/Window.hpp>

#include "core/EventList.hpp"

#include <xcb/xcb.h>

namespace swizzle::core
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

        virtual bool isVisible() const override;

        virtual void setBorderless(bool borderless) override;

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
        xcb_connection_t* mDisplayConnection;
        xcb_screen_t* mScreen;
        xcb_drawable_t mWindow;

        bool mCursorVisible;

        EventHandlerList<WindowEvent> mEventHandlers;
    };

} // namespace swizzle

#endif

#endif