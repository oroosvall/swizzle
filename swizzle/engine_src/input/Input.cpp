
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
        common::Resource<core::SwWindow> mActiveWindow = nullptr;
        InputCallback mCb = {};
        F32 mDx = 0.0f;
        F32 mDy = 0.0f;
        F32 mScrollDx = 0.0f;
        F32 mScrollDy = 0.0f;
        S32 mX = 0;
        S32 mY = 0;
        std::unordered_map<S32, SwBool> mPressedKeys = {};
        std::unordered_map<S32, SwBool> mPressedMouse = {};

        std::unordered_map<S32, SwBool> mPressedKeysThisFrame = {};
        std::unordered_map<S32, SwBool> mPressedMouseThisFrame = {};

        SwBool mInputFocused = false;

        S32 mCharacterEventValue = 0;
        SwBool mCharacterHadEventThisFrame = false;

        std::unordered_map<core::GamePadAxis, F32> mGamepadAxisValues = {};
        std::unordered_map<core::GamePadButton, SwBool> mGamePadButtonValues = {};

    } inputCtx;
}

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::input
{

    void SetInputSource(common::Resource<core::SwWindow> window)
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

    SwBool RegisterKeyAction(const KeyAction& action)
    {
        UNUSED_ARG(action);
        return false;
    }

    void InputFrameReset()
    {
        OPTICK_EVENT("InputFrameReset");
        inputCtx.mDx = 0.0f;
        inputCtx.mDy = 0.0f;
        inputCtx.mScrollDx = 0.0f;
        inputCtx.mScrollDy = 0.0f;
        inputCtx.mPressedKeysThisFrame.clear();
        inputCtx.mPressedMouseThisFrame.clear();
        inputCtx.mCharacterHadEventThisFrame = false;
    }

    SwBool IsKeyPressed(const Keys key)
    {
        SwBool pressed = false;
        S32 scanCode = swizzle::core::KeyToScanCode(key);
        if (inputCtx.mPressedKeys.count(scanCode) != 0u)
        {
            pressed = inputCtx.mPressedKeys[scanCode];
        }
        return pressed && inputCtx.mInputFocused;
    }

    SwBool WasKeyPressedThisFrame(const Keys key)
    {
        SwBool pressed = false;
        S32 scanCode = swizzle::core::KeyToScanCode(key);
        if (inputCtx.mPressedKeysThisFrame.count(scanCode) != 0u)
        {
            pressed = inputCtx.mPressedKeysThisFrame[scanCode];
        }
        return pressed && inputCtx.mInputFocused;
    }

    SwBool WasKeyReleasedThisFrame(const Keys key)
    {
        SwBool pressed = false;
        S32 scanCode = swizzle::core::KeyToScanCode(key);
        if (inputCtx.mPressedKeysThisFrame.count(scanCode) != 0u)
        {
            pressed = (inputCtx.mPressedKeysThisFrame[scanCode] == false);
        }
        return pressed && inputCtx.mInputFocused;
    }

    SwBool IsActionPressed(const KeyAction& action)
    {
        UNUSED_ARG(action);
        return false;
    }

    SwBool IsMouseButtonPressed(const Mouse key)
    {
        SwBool pressed = false;
        S32 scanCode = (S32)key;
        if (inputCtx.mPressedMouse.count(scanCode) != 0u)
        {
            pressed = inputCtx.mPressedMouse[scanCode];
        }
        return pressed && inputCtx.mInputFocused;
    }

    SwBool WasMousePressedThisFrame(const Mouse key)
    {
        SwBool pressed = false;
        S32 scanCode = (S32)key;
        if (inputCtx.mPressedMouseThisFrame.count(scanCode) != 0u)
        {
            pressed = inputCtx.mPressedMouseThisFrame[scanCode];
        }
        return pressed && inputCtx.mInputFocused;
    }

    SwBool WasMouseReleasedThisFrame(const Mouse key)
    {
        SwBool pressed = false;
        S32 scanCode = (S32)key;
        if (inputCtx.mPressedMouseThisFrame.count(scanCode) != 0u)
        {
            pressed = (inputCtx.mPressedMouseThisFrame[scanCode] == false);
        }
        return pressed && inputCtx.mInputFocused;
    }

    SwBool HasReceivedCharacterEvent()
    {
        return inputCtx.mCharacterHadEventThisFrame;
    }

    S32 GetCharacterEventValue()
    {
        return inputCtx.mCharacterEventValue;
    }

    void GetMousePosition(int32_t& xPos, int32_t& yPos)
    {
        //if (inputCtx.mInputFocused)
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

    void GetMouseScrollDelta(float& x, float& y)
    {
        if (inputCtx.mInputFocused)
        {
            x = inputCtx.mScrollDx;
            y = inputCtx.mScrollDy;
        }
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

namespace swizzle::input
{
    void InputCallback::publishEvent(const core::WindowEvent& evt)
    {
        core::WindowEventType evtType = evt.getEventType();

        switch (evtType)
        {
        case core::WindowEventType::FocusEvent:
        {
            core::WindowFocusEvent& e = (core::WindowFocusEvent&)evt;
            inputCtx.mInputFocused = e.mFocused;
            break;
        }
        case core::WindowEventType::KeyboardInputEvent:
        {
            core::InputEvent& e = (core::InputEvent&)evt;

            if (swizzle::core::KeyToScanCode(Keys::KeyNone) == e.mKey)
            {
                LOG_INFO("Key %d was None", e.mKey);
            }
            if (e.mFromKeyboard)
            {
                inputCtx.mPressedKeys[e.mKey] = e.mPressed;
                inputCtx.mPressedKeysThisFrame[e.mKey] = e.mPressed;
            }
            else
            {
                inputCtx.mPressedMouse[e.mKey] = e.mPressed;
                inputCtx.mPressedMouseThisFrame[e.mKey] = e.mPressed;
            }

            break;
        }
        case core::WindowEventType::CharacterTypeEvent:
        {
            core::CharacterEvent& e = (core::CharacterEvent&)evt;
            inputCtx.mCharacterEventValue = e.mCodePoint;
            inputCtx.mCharacterHadEventThisFrame = true;
            break;
        }
        case core::WindowEventType::MouseMoveEvent:
        {
            core::MouseMoveEvent& e = (core::MouseMoveEvent&)evt;
            inputCtx.mX = e.mX;
            inputCtx.mY = e.mY;
            break;
        }
        case core::WindowEventType::MouseMoveDeltaEvent:
        {
            core::MouseMoveDelta& e = (core::MouseMoveDelta&)evt;
            inputCtx.mDx += (float)e.dX;
            inputCtx.mDy += (float)e.dY;
            break;
        }
        case core::WindowEventType::GamepadAxisEvent:
        {
            core::GamepadAxisEvent& e = (core::GamepadAxisEvent&)evt;
            inputCtx.mGamepadAxisValues[e.mAxis] = e.mAxisValue;
            break;
        }
        case core::WindowEventType::GamepadButtonEvent:
        {
            core::GamepadButtonEvent& e = (core::GamepadButtonEvent&)evt;
            inputCtx.mGamePadButtonValues[e.mButton] = e.mButtonPressed;
            break;
        }
        case core::WindowEventType::MouseScrollEvent:
        {
            core::MouseScrollEvent& e = (core::MouseScrollEvent&)evt;
            inputCtx.mScrollDx += (float)e.mScrollX;
            inputCtx.mScrollDy += (float)e.mScrollY;
            break;
        }
        default:
            break;
        }
    }
}

/* Class Private Function Definition */

