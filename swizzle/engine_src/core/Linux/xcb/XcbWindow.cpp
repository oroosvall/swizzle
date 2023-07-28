#ifdef SW_LINUX_XCB

#include <cstdlib>
#include "XcbWindow.hpp"

namespace swizzle::core
{
xcb_gcontext_t foreground;
    XcbWindow::XcbWindow(const U32 width, const U32 height, const char* title)
        : mDisplayConnection(nullptr)
        , mScreen(nullptr)
        , mWindow()
    {
        UNUSED_ARG(title);
        mDisplayConnection = xcb_connect(NULL, NULL);

        mScreen = xcb_setup_roots_iterator(xcb_get_setup(mDisplayConnection)).data;
        mWindow = mScreen->root;

        foreground = xcb_generate_id (mDisplayConnection);
        uint32_t mask = 0;
        uint32_t values[2];

        mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
        values[0] = mScreen->black_pixel;
        values[1] = 0;
        xcb_create_gc (mDisplayConnection, foreground, mWindow, mask, values);

        /* create white (background) graphic context */
        xcb_gcontext_t background = xcb_generate_id (mDisplayConnection);
        mask = XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
        values[0] = mScreen->white_pixel;
        values[1] = 0;
        xcb_create_gc (mDisplayConnection, background, mWindow, mask, values);

        /* create the window */
        mWindow = xcb_generate_id(mDisplayConnection);
        mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        values[0] = mScreen->white_pixel;
        values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
        xcb_create_window (mDisplayConnection,             /* connection    */
                            XCB_COPY_FROM_PARENT,          /* depth         */
                            mWindow,                       /* window Id     */
                            mScreen->root,                 /* parent window */
                            0, 0,                          /* x, y          */
                            width, height,                 /* width, height */
                            10,                            /* border_width  */
                            XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class         */
                            mScreen->root_visual,          /* visual        */
                            mask, values);                 /* masks         */

        /* map the window on the screen */
        xcb_map_window (mDisplayConnection, mWindow);

        xcb_flush (mDisplayConnection);

    }

    XcbWindow::~XcbWindow()
    {
        xcb_destroy_window(mDisplayConnection, mWindow);
        xcb_disconnect(mDisplayConnection);
    }

    void XcbWindow::show()
    {

    }

    void XcbWindow::hide()
    {

    }

    void XcbWindow::addEventListener(EventHandler<WindowEvent>* listener)
    {
        UNUSED_ARG(listener);
    }

    void XcbWindow::removeEventListener(EventHandler<WindowEvent>* listener)
    {
        UNUSED_ARG(listener);
    }

    void XcbWindow::setTitle(const char* title)
    {
        UNUSED_ARG(title);
    }

    void XcbWindow::setTitle(const wchar_t* title)
    {
        UNUSED_ARG(title);
    }

    void XcbWindow::setSize(const U32 width, const U32 height)
    {
        const uint32_t values[] = { width, height };
        xcb_configure_window(mDisplayConnection, mWindow, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
    }

    void XcbWindow::setWindowPos(const S32 xPos, const S32 yPos)
    {
        const int32_t values[] = { xPos, yPos };
        xcb_configure_window(mDisplayConnection, mWindow, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
    }

    void XcbWindow::getWindowPos(S32& xPos, S32& yPos)
    {
        xcb_get_geometry_cookie_t cookie;
        xcb_get_geometry_reply_t* reply;

        cookie = xcb_get_geometry(mDisplayConnection, mWindow);
        /* ... do other work here if possible ... */
        if ((reply = xcb_get_geometry_reply(mDisplayConnection, cookie, NULL)))
        {
            xPos = reply->x;
            yPos = reply->y;
        }
        free(reply);
    }

    bool XcbWindow::isVisible() const
    {
        return true;
    }

    bool XcbWindow::hasFocus() const
    {
        bool focus = false;
        xcb_get_input_focus_cookie_t cookie;
        xcb_get_input_focus_reply_t* reply;

        cookie = xcb_get_input_focus(mDisplayConnection);
        if ((reply = xcb_get_input_focus_reply(mDisplayConnection, cookie, NULL)))
        {
            focus = (mWindow == reply->focus);
        }
        free(reply);

        return focus;
    }

    void XcbWindow::setBorderless(bool borderless)
    {
        UNUSED_ARG(borderless);
    }

    void XcbWindow::setFullscreen(bool fullscreen)
    {
        UNUSED_ARG(fullscreen);
    }

    void XcbWindow::setCursorPos(const S32 xPos, const S32 yPos)
    {
        UNUSED_ARG(xPos);
        UNUSED_ARG(yPos);
    }

    void XcbWindow::getCursorPos(S32& xPos, S32& yPos) const
    {
        xPos = 0u;
        yPos = 0u;
    }

    void XcbWindow::getSize(U32& width, U32& height) const
    {
        xcb_get_geometry_cookie_t cookie;
        xcb_get_geometry_reply_t *reply;

        cookie = xcb_get_geometry(mDisplayConnection, mWindow);
        /* ... do other work here if possible ... */
        if ((reply = xcb_get_geometry_reply(mDisplayConnection, cookie, NULL))) {
            width = reply->width;
            height = reply->height;
        }
        free(reply);
    }

    void* XcbWindow::getNativeWindowHandle() const
    {
        U64 wnd = mWindow;
        return (void*)wnd;
    }

    void* XcbWindow::getNativeDisplayHandle() const
    {
        return mDisplayConnection;
    }

    void XcbWindow::pollEvents()
    {
        xcb_generic_event_t *event;

        while ((event = xcb_poll_for_event (mDisplayConnection)))
        {
            switch (event->response_type & ~0x80) {
            case XCB_EXPOSE:
            {
                xcb_rectangle_t rectangles[] = {
                    {40, 40, 20, 20},
                };

                xcb_poly_rectangle (mDisplayConnection, mWindow, foreground, 1, rectangles);
                xcb_flush (mDisplayConnection);
                break;
            }
            case XCB_CONFIGURE_NOTIFY:
            {
                xcb_configure_notify_event_t* ev = (xcb_configure_notify_event_t*)event;
                WindowResizeEvent e;
                e.mWindow = this;
                e.mWidth = ev->width;
                e.mHeight = ev->height;
                mEventHandlers.publishEvent(e);
                break;
            }
            case XCB_KEY_PRESS:
                break;
            }
            free (event);
        }

    }

    void XcbWindow::setCursorVisible(bool visible)
    {
        UNUSED_ARG(visible);
    }

    bool XcbWindow::isCursorVisible() const
    {
        return true;
    }

    EventHandlerList<WindowEvent>& XcbWindow::getEventHandler()
    {
        return mEventHandlers;
    }

}

#endif