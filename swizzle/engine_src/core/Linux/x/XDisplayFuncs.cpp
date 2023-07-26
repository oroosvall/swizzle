#ifdef SW_LINUX_XLIB

/* Include files */

#include <common/Common.hpp>
#include <swizzle/core/Window.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::core
{
    namespace platform
    {
        U32 getMonitorCount()
        {
            Display* disp = XOpenDisplay(NULL);
            int dispCount = XScreenCount(disp);
            XCloseDisplay(disp);

            return static_cast<U32>(dispCount);
        }

        MonitorInfo getMonitorInfo(U32 index)
        {
            Display* disp = XOpenDisplay(NULL);
            int dispCount = XScreenCount(disp);

            MonitorInfo res{};
            int idx = static_cast<S32>(index);
            if (idx < dispCount)
            {
                Screen* s = XScreenOfDisplay(disp, idx);

                Window root = RootWindowOfScreen(s);

                XWindowAttributes xwa{};
                XGetWindowAttributes(disp, root, &xwa);
                U32 xPos = static_cast<U32>(xwa.x);
                U32 yPos = static_cast<U32>(xwa.y);

                res.mWidth = s->width;
                res.mHeight = s->height;
                res.mXPos = xPos;
                res.mYPos = yPos;

                printf("(%d,%d) -> (%d,%d)\n", res.mXPos, res.mYPos, res.mWidth, res.mHeight);
            }

            XCloseDisplay(disp);
            return res;
        }
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */

#endif