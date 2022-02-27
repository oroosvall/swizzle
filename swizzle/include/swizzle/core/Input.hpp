#ifndef INPUT_HPP
#define INPUT_HPP

/* Include files */

#include <common/Common.hpp>
#include "swizzle/Api.hpp"
#include "swizzle/core/Window.hpp"

#include <cstdint>

/* Defines */

/* Typedefs/enums */

namespace swizzle::input
{
    enum class Keys
    {
        KeyNone = -1,
        KeyEsc,

        Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,

        KeyHyphen, KeyEquals,

        KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ,
        KeyK, KeyL, KeyM, KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS, KeyT,
        KeyU, KeyV, KeyW, KeyX, KeyY, KeyZ,

        KeyLBracket, KeyRBracket, KeyEnter,
        KeySemiColon, KeyApostrophe, KeyBackslash, KeyForwardslash,

        KeySpace, KeyTilde, KeyPause, KeyScrollLock,
        KeySysReq, KeyNumPad, KeyApplication,

        KeyArrowUp, KeyArrowDown, KeyArrowLeft, KeyArrowRight,

        KeyF1, KeyF2, KeyF3, KeyF4, KeyF5, KeyF6,
        KeyF7, KeyF8, KeyF9, KeyF10, KeyF11, KeyF12,

        KeyLCtrl, KeyRCtrl,
        KeyLShift, KeyRShift,
        KeyLAlt, KeyRAlt,
        KeyLSpecial, KeyRSpecial,

        KeyTab, KeyCaps, KeyPipe,
        KeyComma, KeyPeriod,

        KeyNumPad0, KeyNumPad1, KeyNumPad2,
        KeyNumPad3, KeyNumPad4, KeyNumPad5,
        KeyNumPad6, KeyNumPad7, KeyNumPad8,
        KeyNumPad9,

        KeyDelete, KeyInsert, KeyHome,
        KeyEnd, KeyPageUp, KeyPageDown,

        KeyBackspace,
        KeyNumPadEnter,
        KeyNumPadPlus,
        KeyNumPadHyphon,
        KeyNumPadAsterisk,
        KeyNumPadSlash,
        KeyNumPadDelete,
    };

    enum class Mouse
    {
        RightClick,
        LeftClick,
        MiddleClick,
        ExtButton3,
        ExtButton4,
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::input
{
    struct Modifiers
    {
        bool mCtrl;
        bool mShift;
        bool mAlt;
        bool mSpecial;
    };

    struct KeyAction
    {
        Keys mKey;
        Keys mOptionalKey;
        Modifiers mMainMods;
        Modifiers mOptionalMods;
    };

}

/* Function Declaration*/

namespace swizzle::input
{
    void SWIZZLE_API SetInputSource(common::Resource<core::SwWindow> window);

    SwBool SWIZZLE_API RegisterKeyAction(const KeyAction& action);

    void SWIZZLE_API InputFrameReset();

    SwBool SWIZZLE_API IsKeyPressed(const Keys key);
    SwBool SWIZZLE_API IsActionPressed(const KeyAction& action);
    SwBool SWIZZLE_API WasKeyPressedThisFrame(const Keys key);
    SwBool SWIZZLE_API WasKeyReleasedThisFrame(const Keys key);

    SwBool SWIZZLE_API IsMouseButtonPressed(const Mouse key);
    SwBool SWIZZLE_API WasMousePressedThisFrame(const Mouse key);
    SwBool SWIZZLE_API WasMouseReleasedThisFrame(const Mouse key);

    SwBool SWIZZLE_API HasReceivedCharacterEvent();
    S32 SWIZZLE_API GetCharacterEventValue();

    void SWIZZLE_API GetMousePosition(int32_t& xPos, int32_t& yPos);
    void SWIZZLE_API GetMouseDelta(float& x, float& y);

}


/* Class Declaration */

#endif