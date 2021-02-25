
/* Include files */

#include "swizzle/core/Input.hpp"
#include "swizzle/Swizzle.hpp"

#include <unordered_map>

/* Defines */

/* Typedefs */

/* Structs/Classes */

namespace swizzle::input
{
    class InputCallback : public EventHandler<core::WindowEvent>
    {
    protected:
        virtual void publishEvent(const core::WindowEvent& evt) override;
    };

    static struct
    {
        core::Resource<core::Window> mActiveWindow = nullptr;
        InputCallback mCb = {};
        F32 mDx = 0.0F;
        F32 mDy = 0.0F;
        S32 mX = 0;
        S32 mY = 0;
        std::unordered_map<S32, SwBool> mPressedKeys = {};
        std::unordered_map<S32, SwBool> mPressedMouse = {};
        SwBool mInputFocused = false;
    } inputCtx;
}

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::input
{

    void SetInputSource(core::Resource<core::Window> window)
    {
        if (inputCtx.mActiveWindow)
        {
            inputCtx.mActiveWindow->removeEventListener(&inputCtx.mCb);
        }
        inputCtx.mActiveWindow = window;
        if (window)
        {
            window->addEventListener(&inputCtx.mCb);
        }
    }

    bool RegisterKeyAction(const KeyAction& action)
    {
        action;
        return false;
    }

    void InputFrameReset()
    {
        inputCtx.mDx = 0.0F;
        inputCtx.mDy = 0.0F;
    }

    bool IsKeyPressed(const Keys key)
    {
        bool pressed = false;
        S32 scanCode = swizzle::core::KeyToScanCode(key);
        if (inputCtx.mPressedKeys.count(scanCode) != 0u)
        {
            pressed = inputCtx.mPressedKeys[scanCode];
        }
        return pressed && inputCtx.mInputFocused;
    }

    bool IsActionPressed(const KeyAction& action)
    {
        action;
        return false;
    }

    bool SWIZZLE_API IsMouseButtonPressed(const Mouse key)
    {
        bool pressed = false;
        S32 scanCode = (S32)key;
        if (inputCtx.mPressedMouse.count(scanCode) != 0u)
        {
            pressed = inputCtx.mPressedMouse[scanCode];
        }
        return pressed && inputCtx.mInputFocused;
    }

    void GetMousePosition(int32_t& xPos, int32_t& yPos)
    {
        if (inputCtx.mInputFocused)
        {
            xPos = inputCtx.mX;
            yPos = inputCtx.mY;
        }
    }

    void GetMouseDelta(float& x, float& y)
    {
        if (inputCtx.mInputFocused)
        {
            x = inputCtx.mDx;
            y = inputCtx.mDy;
        }
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

namespace swizzle ::input
{
    void InputCallback::publishEvent(const core::WindowEvent& evt)
    {
        core::WindowEventType evtType = evt.getEventType();

        switch (evtType)
        {
        case core::WindowEventType::FocusEvent: {
            core::WindowFocusEvent& e = (core::WindowFocusEvent&)evt;
            inputCtx.mInputFocused = e.mFocused;
            break;
        }
        case core::WindowEventType::KeyboardInputEvent: {
            core::InputEvent& e = (core::InputEvent&)evt;

            if (swizzle::core::KeyToScanCode(Keys::KeyNone) == e.mKey)
            {
                LOG_INFO("Key %d was None", e.mKey);
            }
            if (e.mFromKeyboard)
            {
                inputCtx.mPressedKeys[e.mKey] = e.mPressed;
            }
            else
            {
                inputCtx.mPressedMouse[e.mKey] = e.mPressed;
            }

            break;
        }
        case core::WindowEventType::CharacterTypeEvent:
            break;
        case core::WindowEventType::MouseMoveEvent: {
            core::MouseMoveEvent& e = (core::MouseMoveEvent&)evt;
            inputCtx.mX = e.mX;
            inputCtx.mY = e.mY;
            break;
        }
        case core::WindowEventType::MouseMoveDeltaEvent: {
            core::MouseMoveDelta& e = (core::MouseMoveDelta&)evt;
            inputCtx.mDx += (float)e.dX;
            inputCtx.mDy += (float)e.dY;
            break;
        }
        case core::WindowEventType::MouseScrollEvent:
            break;
        default:
            break;
        }
    }
}

/* Class Private Function Definition */

