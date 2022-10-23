
/* Include files */

#include "Win32Xinput.hpp"

/* Defines */

/* Typedefs */

namespace win32
{
    namespace sCore = swizzle::core;
}

/* Structs/Classes */

/* Static Function Declaration */

namespace win32
{
    static inline void sendAxisEvent(swizzle::EventHandlerList<sCore::WindowEvent>& evtHandler, sCore::GamePadAxis ax,
                                     S16 value);
    static inline void sendTriggerEvent(swizzle::EventHandlerList<sCore::WindowEvent>& evtHandler,
                                        sCore::GamePadAxis ax, U8 value);
    static inline void sendButtonEvent(swizzle::EventHandlerList<sCore::WindowEvent>& evtHandler,
                                       sCore::GamePadButton btn, SwBool pressed);
} // namespace win32

/* Static Variables */

/* Static Function Definition */

namespace win32
{
    static inline void sendAxisEvent(swizzle::EventHandlerList<sCore::WindowEvent>& evtHandler, sCore::GamePadAxis ax,
                                     S16 value)
    {
        sCore::GamepadAxisEvent axEvt{};
        axEvt.mAxis = ax;
        axEvt.mAxisValue = F32(value);
        const F32 deadzone = 2048.0f;
        if (abs(axEvt.mAxisValue) > deadzone)
        {
            axEvt.mAxisValue /= 32767.0f;
        }
        else
        {
            axEvt.mAxisValue = 0.0f;
        }
        evtHandler.publishEvent(axEvt);
    }

    static inline void sendTriggerEvent(swizzle::EventHandlerList<sCore::WindowEvent>& evtHandler,
                                        sCore::GamePadAxis ax, U8 value)
    {
        sCore::GamepadAxisEvent axEvt{};
        axEvt.mAxis = ax;
        axEvt.mAxisValue = F32(value) / 255.0f;
        evtHandler.publishEvent(axEvt);
    }

    static inline void sendButtonEvent(swizzle::EventHandlerList<sCore::WindowEvent>& evtHandler,
                                       sCore::GamePadButton btn, SwBool pressed)
    {
        sCore::GamepadButtonEvent btnEvt{};
        btnEvt.mButton = btn;
        btnEvt.mButtonPressed = pressed;
        evtHandler.publishEvent(btnEvt);
    }
} // namespace win32

/* Function Definition */

namespace win32
{
    void ProcessXInput(swizzle::EventHandlerList<swizzle::core::WindowEvent>& evtHandler)
    {
        XINPUT_STATE state{};
        DWORD status = XInputGetState(0u, &state);

        static DWORD packet = 0;

        if (status == ERROR_SUCCESS && state.dwPacketNumber != packet)
        {
            sendAxisEvent(evtHandler, sCore::GamePadAxis::Left_X, state.Gamepad.sThumbLX);
            sendAxisEvent(evtHandler, sCore::GamePadAxis::Left_Y, state.Gamepad.sThumbLY);
            sendTriggerEvent(evtHandler, sCore::GamePadAxis::Left_Z, state.Gamepad.bLeftTrigger);

            sendAxisEvent(evtHandler, sCore::GamePadAxis::Right_X, state.Gamepad.sThumbRX);
            sendAxisEvent(evtHandler, sCore::GamePadAxis::Right_Y, state.Gamepad.sThumbRY);
            sendTriggerEvent(evtHandler, sCore::GamePadAxis::Right_Z, state.Gamepad.bRightTrigger);

            sendButtonEvent(evtHandler, sCore::GamePadButton::A, state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
            sendButtonEvent(evtHandler, sCore::GamePadButton::B, state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
            sendButtonEvent(evtHandler, sCore::GamePadButton::X, state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Y, state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);

            sendButtonEvent(evtHandler, sCore::GamePadButton::LThumb,
                            state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
            sendButtonEvent(evtHandler, sCore::GamePadButton::RThumb,
                            state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);

            sendButtonEvent(evtHandler, sCore::GamePadButton::Start, state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Back, state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);

            sendButtonEvent(evtHandler, sCore::GamePadButton::LBump,
                            state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
            sendButtonEvent(evtHandler, sCore::GamePadButton::RBump,
                            state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

            sendButtonEvent(evtHandler, sCore::GamePadButton::Up, state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Left, state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Down, state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Right,
                            state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

            packet = state.dwPacketNumber;
        }
        else
        {
            sendAxisEvent(evtHandler, sCore::GamePadAxis::Left_X, 0);
            sendAxisEvent(evtHandler, sCore::GamePadAxis::Left_Y, 0);
            sendTriggerEvent(evtHandler, sCore::GamePadAxis::Left_Z, 0);

            sendAxisEvent(evtHandler, sCore::GamePadAxis::Right_X, 0);
            sendAxisEvent(evtHandler, sCore::GamePadAxis::Right_Y, 0);
            sendTriggerEvent(evtHandler, sCore::GamePadAxis::Right_Z, 0);

            sendButtonEvent(evtHandler, sCore::GamePadButton::A, false);
            sendButtonEvent(evtHandler, sCore::GamePadButton::B, false);
            sendButtonEvent(evtHandler, sCore::GamePadButton::X, false);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Y, false);

            sendButtonEvent(evtHandler, sCore::GamePadButton::LThumb, false);
            sendButtonEvent(evtHandler, sCore::GamePadButton::RThumb, false);

            sendButtonEvent(evtHandler, sCore::GamePadButton::Start, false);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Back, false);

            sendButtonEvent(evtHandler, sCore::GamePadButton::LBump, false);
            sendButtonEvent(evtHandler, sCore::GamePadButton::RBump, false);

            sendButtonEvent(evtHandler, sCore::GamePadButton::Up, false);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Left, false);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Down, false);
            sendButtonEvent(evtHandler, sCore::GamePadButton::Right, false);

            packet = 0;
        }
    }
} // namespace win32

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
