#ifndef WINDOW_HPP
#define WINDOW_HPP

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

        virtual void setWindowPos(const U32 xPos, const U32 yPos) = 0;
        virtual void getWindowPos(U32& xPos, U32& yPos) = 0;

        virtual bool isVisible() const = 0;

        virtual bool hasFocus() const = 0;

        virtual void setBorderless(bool borderless) = 0;
        virtual void setFullscreen(bool fullscreen) = 0;

        virtual void getCursorPos(U32& xPos, U32& yPos) const = 0;
        virtual void getSize(U32& width, U32& height) const = 0;
        virtual void* getNativeWindowHandle() const = 0;
        virtual void* getNativeDisplayHandle() const = 0;

        virtual void setCursorVisible(SwBool visible) = 0;
        virtual bool isCursorVisible() const = 0;

        virtual void pollEvents() = 0;
    };
} // namespace swizzle

#endif