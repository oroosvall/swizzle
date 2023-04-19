#ifdef SW_LINUX_XLIB

#include "X11Window.hpp"
#include <X11/Xutil.h>

#include <stdio.h>

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
        windowAttributes.event_mask = KeyPressMask | KeyReleaseMask | StructureNotifyMask | ExposureMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask;

        mWindow = XCreateWindow(mDisplay, RootWindow(mDisplay, s), 0, 0, width,
            height, 0, visualInfo->depth, InputOutput, visualInfo->visual,
            CWBackPixel | CWBorderPixel | CWEventMask | CWColormap, &windowAttributes);

        // XSelectInput(mDisplay, mWindow, KeyPressMask | KeyReleaseMask | StructureNotifyMask | ExposureMask);
        XMapWindow(mDisplay, mWindow);

        mWmDeleteWindow = XInternAtom(mDisplay, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(mDisplay, mWindow, &mWmDeleteWindow, 1);

        XFlush(mDisplay);
        mVisible = true;
        mXLast = mYLast = 0;
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
        mEventHandlers.addListener(listener);
    }

    void XlibWindow::removeEventListener(EventHandler<WindowEvent>* listener)
    {
        mEventHandlers.removeListener(listener);
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
        XResizeWindow(mDisplay, mWindow, width, height);
    }

    bool XlibWindow::isVisible() const
    {
        return mVisible;
    }

    void XlibWindow::setBorderless(bool borderless)
    {
        UNUSED_ARG(borderless);
    }

    void XlibWindow::setFullscreen(bool fullscreen)
    {
        UNUSED_ARG(fullscreen);
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
                WindowResizeEvent e{};
                e.mHeight = evt.xconfigure.height;
                e.mWidth = evt.xconfigure.width;
                mEventHandlers.publishEvent(e);
                break;
            }
            case EnterNotify:
            {
                WindowFocusEvent e{};
                e.mFocused = true;
                mEventHandlers.publishEvent(e);
                break;
            }
            case LeaveNotify:
            {
                WindowFocusEvent e{};
                e.mFocused = false;
                mEventHandlers.publishEvent(e);
                break;
            }
            case MotionNotify:
            {
                MouseMoveEvent eMove {};
                eMove.mX = evt.xmotion.x;
                eMove.mY = evt.xmotion.y;
                mEventHandlers.publishEvent(eMove);

                MouseMoveDelta eDelta{};
                eDelta.dX = evt.xmotion.x - mXLast;
                eDelta.dY = evt.xmotion.y - mYLast;

                mXLast = evt.xmotion.x;
                mYLast = evt.xmotion.y;
                mEventHandlers.publishEvent(eDelta);

                break;
            }
            case ButtonPress:
            {
                InputEvent e;
                e.mPressed = true;
                e.mFromKeyboard = false;
                e.mModKeys = 0;
                e.mKey = evt.xbutton.button;
                mEventHandlers.publishEvent(e);
                break;
            }
            case ButtonRelease:
            {
                InputEvent e;
                e.mPressed = false;
                e.mFromKeyboard = false;
                e.mModKeys = 0;
                e.mKey = evt.xbutton.button;
                mEventHandlers.publishEvent(e);
                break;
            }
            case KeyPress:
            {
                InputEvent e;
                e.mPressed = true;
                e.mFromKeyboard = true;
                e.mModKeys = 0;
                e.mKey = evt.xkey.keycode;
                mEventHandlers.publishEvent(e);
                break;
            }
            case KeyRelease:
            {
                InputEvent e;
                e.mPressed = false;
                e.mFromKeyboard = true;
                e.mModKeys = 0;
                e.mKey = evt.xkey.keycode;
                mEventHandlers.publishEvent(e);
                break;
            }
            case ClientMessage:
            {
                if ((Atom)evt.xclient.data.l[0] == mWmDeleteWindow)
                {
                    mVisible = false;
                }
                break;
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