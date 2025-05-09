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

        KeySpace, KeyGraveAccent, KeyPause, KeyScrollLock,
        KeySysReq, KeyNumLock, KeyApplication,

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
        KeyNumPadAdd,
        KeyNumPadSubtract,
        KeyNumPadMultiply,
        KeyNumPadDivide,
        KeyNumPadDelete,
        COUNT
    };

    enum class Mouse
    {
        None = 0,
        LeftClick,
        RightClick,
        MiddleClick,
        ExtButton4,
        ExtButton5,
        COUNT
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

    void SWIZZLE_API InputFrameReset();

    SwBool SWIZZLE_API IsKeyPressed(const Keys key);
    SwBool SWIZZLE_API IsKeyPressedMods(const Keys key, const Modifiers mods);
    SwBool SWIZZLE_API WasKeyPressedThisFrame(const Keys key);
    SwBool SWIZZLE_API WasKeyPressedThisFrameMods(const Keys key, const Modifiers mods);
    SwBool SWIZZLE_API WasKeyReleasedThisFrame(const Keys key);
    SwBool SWIZZLE_API WasKeyReleasedThisFrameMods(const Keys key, const Modifiers mods);

    SwBool SWIZZLE_API IsMouseButtonPressed(const Mouse key);
    SwBool SWIZZLE_API WasMousePressedThisFrame(const Mouse key);
    SwBool SWIZZLE_API WasMouseReleasedThisFrame(const Mouse key);

    SwBool SWIZZLE_API HasReceivedCharacterEvent();
    S32 SWIZZLE_API GetCharacterEventValue();

    void SWIZZLE_API GetMousePosition(int32_t& xPos, int32_t& yPos);
    void SWIZZLE_API GetMouseDelta(float& x, float& y);
    void SWIZZLE_API GetMouseScrollDelta(float& x, float& y);

    void SWIZZLE_API RegisterKeyAction(const SwChar* actionName, KeyAction& action);
    const KeyAction SWIZZLE_API & GetKeyAction(const SwChar* actionName);
    SwBool SWIZZLE_API IsKeyActionPressed(const SwChar* actionName);
    SwBool SWIZZLE_API WasKeyActionPressedThisFrame(const SwChar* actionName);
    SwBool SWIZZLE_API WasKeyActionReleasedThisFrame(const SwChar* actionName);
}


/* Class Declaration */

#endif