#ifdef SW_LINUX_XLIB

/* Include files */

#include "X11Window.hpp"
#include <X11/Xutil.h>

#include <stdio.h>

#include <unordered_map>

/* Defines */

#define MWM_HINTS_DECORATIONS 2
#define MWM_DECOR_ALL 1

/* Typedefs */

namespace core = swizzle::core;

/* Structs/Classes */

/* Static Variables */

namespace plf::window
{
    Display* gDisplayConnection = nullptr;
    std::unordered_map<Window, x11::X11Window*> gEventMapping;
}

/* Static Function Declaration */

namespace plf::window
{
    void registerWindow(Window wnd, x11::X11Window* target);
    void unregisterWindow(Window wnd);
    void mapEvent(XEvent& evt);
}

/* Static Function Definition */

namespace plf::window
{
    void registerWindow(Window wnd, x11::X11Window* target)
    {
        gEventMapping[wnd] = target;
    }

    void unregisterWindow(Window wnd)
    {
        gEventMapping.erase(wnd);
    }

    void mapEvent(XEvent& evt)
    {
        Window wnd = evt.xany.window;
        if(gEventMapping.count(wnd))
        {
            gEventMapping[wnd]->processEvents(evt);
        }
    }
}

/* Function Definition */

namespace plf::window
{
    SwBool init()
    {
        XInitThreads();

        gDisplayConnection = XOpenDisplay(nullptr);
        return gDisplayConnection != nullptr;
    }

    void cleanup()
    {
        XCloseDisplay(gDisplayConnection);
        gDisplayConnection = nullptr;
    }

    void pollWindowEvents()
    {
        XEvent evt;
        while (XPending(gDisplayConnection) > 0)
        {
            XNextEvent(gDisplayConnection, &evt);
            mapEvent(evt);
        }
    }
}

namespace x11
{
    common::Resource<X11Window> createWindow(const U32 width, const U32 height, const SwChar* title)
    {
        return std::make_shared<X11Window>(plf::window::gDisplayConnection, width, height, title);
    }
}

/* Class Public Function Definition */

namespace x11
{

    S32 screen = 0;

    X11Window::X11Window(Display* disp, const U32 width, const U32 height, const char* title)
        : mDisplay(disp)
        , mWindow()
    {
        UNUSED_ARG(title);

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
        windowAttributes.event_mask = KeyPressMask | KeyReleaseMask | StructureNotifyMask | ExposureMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask;

        mWindow = XCreateWindow(mDisplay, RootWindow(mDisplay, s), 0, 0, width,
            height, 0, visualInfo->depth, InputOutput, visualInfo->visual,
            CWBackPixel | CWBorderPixel | CWEventMask | CWColormap, &windowAttributes);

        plf::window::registerWindow(mWindow, this);

        // XSelectInput(mDisplay, mWindow, KeyPressMask | KeyReleaseMask | StructureNotifyMask | ExposureMask);
        XMapWindow(mDisplay, mWindow);

        mWmDeleteWindow = XInternAtom(mDisplay, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(mDisplay, mWindow, &mWmDeleteWindow, 1);

        mWmDecorations = XInternAtom(mDisplay, "_MOTIF_WM_HINTS", True);

        XFlush(mDisplay);
        mVisible = true;
        mLastCursorXPos = mLastCursorYPos = 0;
    }

    X11Window::~X11Window()
    {
        plf::window::unregisterWindow(mWindow);
        XDestroyWindow(mDisplay, mWindow);
    }

    void X11Window::show()
    {

    }

    void X11Window::hide()
    {

    }

    void X11Window::addEventListener(EventHandler<WindowEvent>* listener)
    {
        mEventHandlers.addListener(listener);
    }

    void X11Window::removeEventListener(EventHandler<WindowEvent>* listener)
    {
        mEventHandlers.removeListener(listener);
    }

    void X11Window::setTitle(const char* title)
    {
        UNUSED_ARG(title);
    }

    void X11Window::setTitle(const wchar_t* title)
    {
        UNUSED_ARG(title);
    }

    void X11Window::setSize(const U32 width, const U32 height)
    {
        XResizeWindow(mDisplay, mWindow, width, height);
        XFlush(mDisplay);
    }

    void X11Window::setWindowPos(const S32 xPos, const S32 yPos)
    {
        XMoveWindow(mDisplay, mWindow, xPos, yPos);
        XFlush(mDisplay);
    }

    void X11Window::getWindowPos(S32& xPos, S32& yPos)
    {
        Window w;
        Window root = XDefaultRootWindow(mDisplay);

        XTranslateCoordinates(mDisplay, mWindow, root, 0, 0, &xPos, &yPos, &w);
    }

    bool X11Window::isVisible() const
    {
        return mVisible;
    }

    bool X11Window::hasFocus() const
    {
        Window w;
        int revertTo;
        XGetInputFocus(mDisplay, &w, &revertTo);
        return w == mWindow;
    }

    void X11Window::setBorderless(bool borderless)
    {
        
        struct {
            unsigned long flags;
            unsigned long functions;
            unsigned long decorations;
        } hints = {0};

        hints.flags = MWM_HINTS_DECORATIONS;
        hints.decorations = !borderless ? MWM_DECOR_ALL : 0;

        UNUSED_ARG(borderless);
        XChangeProperty(mDisplay, mWindow, mWmDecorations, mWmDecorations, 32, PropModeReplace, (unsigned char*)&hints, 3);
    }

    void X11Window::setFullscreen(bool fullscreen)
    {
        UNUSED_ARG(fullscreen);
    }

    void X11Window::setCursorPos(const S32 xPos, const S32 yPos) 
    {
        XWarpPointer(mDisplay, None, mWindow, 0, 0, 0u, 0u, xPos, yPos);
        XFlush(mDisplay);
    }

    void X11Window::getCursorPos(S32& xPos, S32& yPos) const
    {
        Window root;
        Window child;

        int rx = 0, ry = 0; 
        int x = 0, y = 0;

        unsigned int mods = 0;

        XQueryPointer(mDisplay, mWindow, &root, &child, &rx, &ry, &x, &y, &mods);

        xPos = x;
        yPos = y;
    }

    void X11Window::getSize(U32& width, U32& height) const
    {
        XWindowAttributes attribs;
        XGetWindowAttributes(mDisplay, mWindow, &attribs);
        width = attribs.width;
        height = attribs.height;
    }

    void* X11Window::getNativeWindowHandle() const
    {
        return (void*)mWindow;
    }

    void* X11Window::getNativeDisplayHandle() const
    {
        return mDisplay;
    }

    void X11Window::pollEvents()
    {
        XEvent evt;
        while (XPending(mDisplay) > 0)
        {
            XNextEvent(mDisplay, &evt);
            processEvents(evt);
        }
    }

    void X11Window::setCursorVisible(bool visible)
    {
        UNUSED_ARG(visible);
    }

    bool X11Window::isCursorVisible() const
    {
        return true;
    }

    EventHandlerList<WindowEvent>& X11Window::getEventHandler()
    {
        return mEventHandlers;
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace x11
{
    void X11Window::processEvents(XEvent& evt)
    {
        switch (evt.type)
        {
        case ConfigureNotify:
        {
            core::WindowResizeEvent e{};
            e.mWindow = this;
            e.mHeight = evt.xconfigure.height;
            e.mWidth = evt.xconfigure.width;
            mEventHandlers.publishEvent(e);

            core::WindowMoveEvent eMove{};
            eMove.mWindow = this;
            eMove.mXPos = evt.xconfigure.x;
            eMove.mYPos = evt.xconfigure.y;
            mEventHandlers.publishEvent(eMove);

            break;
        }
        case EnterNotify:
        {
            const int xPos = evt.xcrossing.x;
            const int yPos = evt.xcrossing.y;

            core::MouseEnterEvent e{};
            e.mWindow = this;
            e.mEnter = true;
            mEventHandlers.publishEvent(e);

            core::MouseMoveEvent eMove{};
            eMove.mWindow = this;
            eMove.mX = xPos;
            eMove.mY = yPos;
            mEventHandlers.publishEvent(e);

            mLastCursorXPos = xPos;
            mLastCursorYPos = yPos;

            break;
        }
        case LeaveNotify:
        {
            core::MouseEnterEvent e{};
            e.mWindow = this;
            e.mEnter = false;
            mEventHandlers.publishEvent(e);
            break;
        }
        case FocusIn:
        {
            core::WindowFocusEvent e{};
            e.mWindow = this;
            e.mFocused = true;
            mEventHandlers.publishEvent(e);
            break;
        }
        case FocusOut:
        {
            core::WindowFocusEvent e{};
            e.mWindow = this;
            e.mFocused = false;
            mEventHandlers.publishEvent(e);
            break;
        }
        case MotionNotify:
        {
            core::MouseMoveEvent eMove {};
            eMove.mWindow = this;
            eMove.mX = evt.xmotion.x;
            eMove.mY = evt.xmotion.y;
            mEventHandlers.publishEvent(eMove);

            core::MouseMoveDelta eDelta{};
            eDelta.mWindow = this;
            eDelta.dX = evt.xmotion.x - mLastCursorXPos;
            eDelta.dY = evt.xmotion.y - mLastCursorYPos;

            mLastCursorXPos = evt.xmotion.x;
            mLastCursorYPos = evt.xmotion.y;
            mEventHandlers.publishEvent(eDelta);

            break;
        }
        case ButtonPress:
        {
            core::InputEvent e;
            e.mWindow = this;
            e.mPressed = true;
            e.mFromKeyboard = false;
            e.mModKeys = 0;
            e.mKey = evt.xbutton.button;
            mEventHandlers.publishEvent(e);
            break;
        }
        case ButtonRelease:
        {
            core::InputEvent e;
            e.mWindow = this;
            e.mPressed = false;
            e.mFromKeyboard = false;
            e.mModKeys = 0;
            e.mKey = evt.xbutton.button;
            mEventHandlers.publishEvent(e);
            break;
        }
        case KeyPress:
        {
            core::InputEvent e;
            e.mWindow = this;
            e.mPressed = true;
            e.mFromKeyboard = true;
            e.mModKeys = 0;
            e.mKey = evt.xkey.keycode;
            mEventHandlers.publishEvent(e);
            break;
        }
        case KeyRelease:
        {
            core::InputEvent e;
            e.mWindow = this;
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
                core::WindowCloseEvent e;
                e.mWindow = this;
                mEventHandlers.publishEvent(e);

                mVisible = false;
            }
            break;
        }
        default:
            break;
        }
    }
}

#endif