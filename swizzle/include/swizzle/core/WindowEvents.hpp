#ifndef WINDOW_EVENTS_HPP
#define WINDOW_EVENTS_HPP

#include <swizzle/core/Types.hpp>
#include <swizzle/Api.hpp>

namespace swizzle::core
{
    enum class WindowEventType
    {
        ResizeEvent,
        FocusEvent,
        CloseEvent,
        KeyboardInputEvent,
        CharacterTypeEvent,
        MouseMoveEvent,
        MouseMoveDeltaEvent,
        MouseScrollEvent
    };

#define MODKEY_SHIFT 0x01
#define MODKEY_CONTROL 0x02
#define MODKEY_ALT 0x04
#define MODKEY_SUPER 0x08

    class WindowEvent
    {
    public:
        virtual const WindowEventType getEventType() const = 0;
    };

    class WindowResizeEvent : public WindowEvent
    {
    public:
        virtual const WindowEventType getEventType() const override
        {
            return WindowEventType::ResizeEvent;
        }
        S32 mWidth;
        S32 mHeight;
    };

    class WindowFocusEvent : public WindowEvent
    {
    public:
        virtual const WindowEventType getEventType() const override
        {
            return WindowEventType::FocusEvent;
        }
        SwBool mFocused;
    };

    class WindowCloseEvent : public WindowEvent
    {
    public:
        virtual const WindowEventType getEventType() const override
        {
            return WindowEventType::CloseEvent;
        }
    };

    class InputEvent : public WindowEvent
    {
    public:
        virtual const WindowEventType getEventType() const override
        {
            return WindowEventType::KeyboardInputEvent;
        }

        SwBool mFromKeyboard;
        S32 mKey;
        S32 mModKeys;
        SwBool mPressed;
    };

    class CharacterEvent : public WindowEvent
    {
    public:
        virtual const WindowEventType getEventType() const override
        {
            return WindowEventType::CharacterTypeEvent;
        }
        U32 mCodePoint;
    };

    class MouseMoveEvent : public WindowEvent
    {
    public:
        virtual const WindowEventType getEventType() const override
        {
            return WindowEventType::MouseMoveEvent;
        }

        S32 mX;
        S32 mY;
    };

    class MouseMoveDelta : public WindowEvent
    {
    public:
        virtual const WindowEventType getEventType() const override
        {
            return WindowEventType::MouseMoveDeltaEvent;
        }

        S32 dX;
        S32 dY;
    };

    class MouseScrollEvent : public WindowEvent
    {
    public:
        virtual const WindowEventType getEventType() const override
        {
            return WindowEventType::MouseScrollEvent;
        }

        S32 mScrollX;
        S32 mScrollY;
    };

} // namespace swizzle

#endif