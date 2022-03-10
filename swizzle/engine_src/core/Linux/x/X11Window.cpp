#ifdef SW_LINUX_XLIB

#include "X11Window.hpp"
#include <X11/Xutil.h>

namespace swizzle::core
{

    S32 screen = 0;

    XlibWindow::XlibWindow(const U32 width, const U32 height, const char* title)
        : mDisplay(nullptr)
        , mWindow()
    {
        UNUSED_ARG(title);
        XInitThreads();

        mDisplay = XOpenDisplay(NULL);

        long visualMask = VisualScreenMask;
        int numberOfVisuals;
        XVisualInfo vInfoTemplate = {};

        S32 s = DefaultScreen(mDisplay);
        screen = s;
        XVisualInfo* visualInfo = XGetVisualInfo(mDisplay, visualMask, &vInfoTemplate, &numberOfVisuals);

        Colormap colormap =
            XCreateColormap(mDisplay, RootWindow(mDisplay, s), visualInfo->visual, AllocNone);

        XSetWindowAttributes windowAttributes = {};
        windowAttributes.colormap = colormap;
        windowAttributes.background_pixel = 0xFFFFFFFF;
        windowAttributes.border_pixel = 0;
        windowAttributes.event_mask = KeyPressMask | KeyReleaseMask | StructureNotifyMask | ExposureMask;

        mWindow = XCreateWindow(mDisplay, RootWindow(mDisplay, s), 0, 0, width,
            height, 0, visualInfo->depth, InputOutput, visualInfo->visual,
            CWBackPixel | CWBorderPixel | CWEventMask | CWColormap, &windowAttributes);

        XSelectInput(mDisplay, mWindow, ExposureMask | KeyPressMask);
        XMapWindow(mDisplay, mWindow);
        XFlush(mDisplay);
    }

    XlibWindow::~XlibWindow()
    {
        XDestroyWindow(mDisplay, mWindow);
        XCloseDisplay(mDisplay);
    }

    void XlibWindow::show()
    {

    }

    void XlibWindow::hide()
    {

    }

    void XlibWindow::addEventListener(EventHandler<WindowEvent>* listener)
    {
        UNUSED_ARG(listener);
    }

    void XlibWindow::removeEventListener(EventHandler<WindowEvent>* listener)
    {
        UNUSED_ARG(listener);
    }

    void XlibWindow::setTitle(const char* title)
    {
        UNUSED_ARG(title);
    }

    void XlibWindow::setTitle(const wchar_t* title)
    {
        UNUSED_ARG(title);
    }

    void XlibWindow::setSize(const U32 width, const U32 height)
    {
        UNUSED_ARG(width);
        UNUSED_ARG(height);
    }

    bool XlibWindow::isVisible() const
    {
        return true;
    }

    void XlibWindow::setBorderless(bool borderless)
    {
        UNUSED_ARG(borderless);
    }

    void XlibWindow::getCursorPos(U32& xPos, U32& yPos) const
    {
        xPos = 0u;
        yPos = 0u;
    }

    void XlibWindow::getSize(U32& width, U32& height) const
    {
        XWindowAttributes attribs;
        XGetWindowAttributes(mDisplay, mWindow, &attribs);
        width = attribs.width;
        height = attribs.height;
    }

    void* XlibWindow::getNativeWindowHandle() const
    {
        return (void*)mWindow;
    }

    void* XlibWindow::getNativeDisplayHandle() const
    {
        return mDisplay;
    }

    void XlibWindow::pollEvents()
    {
        XEvent evt;

        while (XPending(mDisplay) > 0)
        {
            XNextEvent(mDisplay, &evt);

            switch (evt.type)
            {
            case ConfigureNotify:
            {
                WindowResizeEvent e;
                e.mHeight = evt.xconfigure.height;
                e.mWidth = evt.xconfigure.width;
                mEventHandlers.publishEvent(e);
            }
            default:
                break;
            }
        }

    }

    void XlibWindow::setCursorVisible(bool visible)
    {
        UNUSED_ARG(visible);
    }

    bool XlibWindow::isCursorVisible() const
    {
        return true;
    }

    EventHandlerList<WindowEvent>& XlibWindow::getEventHandler()
    {
        return mEventHandlers;
    }

}

#endif