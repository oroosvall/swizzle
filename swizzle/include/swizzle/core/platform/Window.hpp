#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <swizzle/core/common/Types.hpp>

#include <swizzle/core/common/Event.hpp>
#include <swizzle/core/common/WindowEvents.hpp>

namespace swizzle::core
{

    struct Resolution
    {
        SwU32 mWidth;
        SwU32 mHeight;
        SwU32 mFrequency;
    };

    class Window
    {
    public:
        virtual ~Window() {}

        virtual void show() = 0;
        virtual void hide() = 0;

        virtual void addEventListener(EventHandler<WindowEvent>* listener) = 0;
        virtual void removeEventListener(EventHandler<WindowEvent>* listener) = 0;

        virtual void setTitle(const SwChar* title) = 0;
        virtual void setTitle(const SwWChar* title) = 0;

        virtual void setSize(const SwU32 width, const SwU32 height) = 0;

        virtual bool isVisible() const = 0;

        virtual void setBorderless(bool borderless) = 0;

        virtual void getCursorPos(SwU32& xPos, SwU32& yPos) const = 0;
        virtual void getSize(SwU32& width, SwU32& height) const = 0;
        virtual void* getNativeHandle() const = 0;

        virtual void setCursorVisible(SwBool visible) = 0;
        virtual bool isCursorVisible() const = 0;

        virtual void pollEvents() = 0;
    };
} // namespace swizzle

#endif