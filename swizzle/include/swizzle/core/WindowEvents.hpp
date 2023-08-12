#ifndef WINDOW_EVENTS_HPP
#define WINDOW_EVENTS_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/Api.hpp>

/* Defines */

#define MODKEY_SHIFT 0x01
#define MODKEY_CONTROL 0x02
#define MODKEY_ALT 0x04
#define MODKEY_SUPER 0x08

/* Typedefs/enums */

namespace swizzle::core
{
    enum class WindowEventType
    {
        ResizeEvent,
        MoveEvent,
        FocusEvent,
        CloseEvent,
        KeyboardInputEvent,
        CharacterTypeEvent,
        MouseMoveEvent,
        MouseMoveDeltaEvent,
        MouseScrollEvent,
        GamepadAxisEvent,
        GamepadButtonEvent,
        MouseEnterEvent,
    };

    enum class GamePadAxis
    {
        Right_X,
        Right_Y,
        Right_Z,
        Left_X,
        Left_Y,
        Left_Z
    };

    enum class GamePadButton
    {
        A,
        B,
        X,
        Y,
        RThumb,
        LThumb,
        RBump,
        LBump,
        Start,
        Back,
        Up,
        Down,
        Left,
        Right
    };
} // namespace swizzle::core

/* Forward Declared Structs/Classes */

namespace swizzle::core
{
    class SwWindow;
}

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::core
{

    class WindowEvent
    {
    public:
        virtual ~WindowEvent() {}
        virtual WindowEventType getEventType() const = 0;
    };

    class WindowResizeEvent : public WindowEvent
    {
    public:
        virtual ~WindowResizeEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::ResizeEvent;
        }
        swizzle::core::SwWindow* mWindow;
        S32 mWidth;
        S32 mHeight;
    };

    class WindowMoveEvent : public WindowEvent
    {
    public:
        virtual ~WindowMoveEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::MoveEvent;
        }
        swizzle::core::SwWindow* mWindow;
        S32 mXPos;
        S32 mYPos;
    };

    class MouseEnterEvent : public WindowEvent
    {
    public:
        virtual ~MouseEnterEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::MouseEnterEvent;
        }
        swizzle::core::SwWindow* mWindow;
        SwBool mEnter;
    };

    class WindowFocusEvent : public WindowEvent
    {
    public:
        virtual ~WindowFocusEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::FocusEvent;
        }
        swizzle::core::SwWindow* mWindow;
        SwBool mFocused;
    };

    class WindowCloseEvent : public WindowEvent
    {
    public:
        virtual ~WindowCloseEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::CloseEvent;
        }
        swizzle::core::SwWindow* mWindow;
    };

    class InputEvent : public WindowEvent
    {
    public:
        virtual ~InputEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::KeyboardInputEvent;
        }
        swizzle::core::SwWindow* mWindow;
        SwBool mFromKeyboard;
        S32 mKey;
        S32 mModKeys;
        SwBool mPressed;
    };

    class CharacterEvent : public WindowEvent
    {
    public:
        virtual ~CharacterEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::CharacterTypeEvent;
        }
        swizzle::core::SwWindow* mWindow;
        U32 mCodePoint;
    };

    class MouseMoveEvent : public WindowEvent
    {
    public:
        virtual ~MouseMoveEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::MouseMoveEvent;
        }

        swizzle::core::SwWindow* mWindow;
        S32 mX;
        S32 mY;
    };

    class MouseMoveDelta : public WindowEvent
    {
    public:
        virtual ~MouseMoveDelta() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::MouseMoveDeltaEvent;
        }

        swizzle::core::SwWindow* mWindow;
        S32 dX;
        S32 dY;
    };

    class MouseScrollEvent : public WindowEvent
    {
    public:
        virtual ~MouseScrollEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::MouseScrollEvent;
        }

        swizzle::core::SwWindow* mWindow;
        S32 mScrollX;
        S32 mScrollY;
    };

    class GamepadAxisEvent : public WindowEvent
    {
    public:
        virtual ~GamepadAxisEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::GamepadAxisEvent;
        }

        swizzle::core::SwWindow* mWindow;
        GamePadAxis mAxis;
        F32 mAxisValue;
    };

    class GamepadButtonEvent : public WindowEvent
    {
    public:
        virtual ~GamepadButtonEvent() {}
        virtual WindowEventType getEventType() const override
        {
            return WindowEventType::GamepadButtonEvent;
        }

        swizzle::core::SwWindow* mWindow;
        GamePadButton mButton;
        SwBool mButtonPressed;
    };

} // namespace swizzle::core

/* Function Declaration */

#endif