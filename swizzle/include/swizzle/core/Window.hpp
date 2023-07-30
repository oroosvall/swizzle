#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <swizzle/Api.hpp>
#include <common/Common.hpp>

#include <swizzle/core/Event.hpp>
#include <swizzle/core/WindowEvents.hpp>

namespace swizzle::core
{

    struct Resolution
    {
        U32 mWidth;
        U32 mHeight;
        U32 mFrequency;
    };

    struct MonitorInfo
    {
        S32 mXPos;
        S32 mYPos;
        U32 mWidth;
        U32 mHeight;
    };

    class SwWindow
    {
    public:
        virtual ~SwWindow() {}

        virtual void show() = 0;
        virtual void hide() = 0;

        virtual void addEventListener(EventHandler<WindowEvent>* listener) = 0;
        virtual void removeEventListener(EventHandler<WindowEvent>* listener) = 0;

        virtual void setTitle(const SwChar* title) = 0;
        virtual void setTitle(const SwWChar* title) = 0;

        virtual void setSize(const U32 width, const U32 height) = 0;

        virtual void setWindowPos(const S32 xPos, const S32 yPos) = 0;
        virtual void getWindowPos(S32& xPos, S32& yPos) = 0;

        virtual bool isVisible() const = 0;

        virtual bool hasFocus() const = 0;

        virtual void setBorderless(bool borderless) = 0;
        virtual void setFullscreen(bool fullscreen) = 0;

        virtual void setCursorPos(const S32 xPos, const S32 yPos) = 0;
        virtual void getCursorPos(S32& xPos, S32& yPos) const = 0;
        virtual void getSize(U32& width, U32& height) const = 0;
        virtual void* getNativeWindowHandle() const = 0;
        virtual void* getNativeDisplayHandle() const = 0;

        virtual void setCursorVisible(SwBool visible) = 0;
        virtual bool isCursorVisible() const = 0;
    };

    /// <summary>
    /// Get number of monitors in the system
    /// </summary>
    /// <returns>
    /// Count of monitors to be used to enumerate MonitorInfo
    /// </returns>
    U32 SWIZZLE_API GetMonitorCount();

    /// <summary>
    /// Get info about a monitor
    /// </summary>
    /// <param name="index">Monitor index to get info about</param>
    /// <returns>
    /// MonitorInfo containing position and size of monitor.
    /// </returns>
    MonitorInfo SWIZZLE_API GetMonitorInfo(U32 index);

    /// <summary>
    /// Create a window
    /// </summary>
    /// <param name="width">Width of the window in pixels</param>
    /// <param name="height">Height of the window in pixels</param>
    /// <param name="title">Title for the window</param>
    /// <returns>
    /// Resource handle to a window
    /// </returns>
    common::Resource<SwWindow> SWIZZLE_API CreateSwWindow(const U32 width, const U32 height, const SwChar* title);

    /// <summary>
    /// Function to poll for window events
    /// This will enumerate all windows for the running program
    /// </summary>
    void SWIZZLE_API PollWindowEvents();

} // namespace swizzle

#endif