
/* Include files */

#include "ImGuiTranslateKey.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

ImGuiKey fromScanCode(S32 scanCode)
{
    switch (scanCode)
    {
    case 0: return ImGuiKey::ImGuiKey_None;
    case 1: return ImGuiKey::ImGuiKey_Escape;
    case 2: return ImGuiKey::ImGuiKey_0;
    case 3: return ImGuiKey::ImGuiKey_1;
    case 4: return ImGuiKey::ImGuiKey_2;
    case 5: return ImGuiKey::ImGuiKey_3;
    case 6: return ImGuiKey::ImGuiKey_4;
    case 7: return ImGuiKey::ImGuiKey_5;
    case 8: return ImGuiKey::ImGuiKey_6;
    case 9: return ImGuiKey::ImGuiKey_7;
    case 10: return ImGuiKey::ImGuiKey_8;
    case 11: return ImGuiKey::ImGuiKey_9;
    case 12: return ImGuiKey::ImGuiKey_Minus;
    case 13: return ImGuiKey::ImGuiKey_Equal;
    case 14: return ImGuiKey::ImGuiKey_Backspace;
    case 15: return ImGuiKey::ImGuiKey_Tab;
    case 16: return ImGuiKey::ImGuiKey_Q;
    case 17: return ImGuiKey::ImGuiKey_W;
    case 18: return ImGuiKey::ImGuiKey_E;
    case 19: return ImGuiKey::ImGuiKey_R;
    case 20: return ImGuiKey::ImGuiKey_T;
    case 21: return ImGuiKey::ImGuiKey_Y;
    case 22: return ImGuiKey::ImGuiKey_U;
    case 23: return ImGuiKey::ImGuiKey_I;
    case 24: return ImGuiKey::ImGuiKey_O;
    case 25: return ImGuiKey::ImGuiKey_P;
    case 26: return ImGuiKey::ImGuiKey_LeftBracket;
    case 27: return ImGuiKey::ImGuiKey_RightBracket;
    case 28: return ImGuiKey::ImGuiKey_Enter;
    case 29: return ImGuiKey::ImGuiKey_LeftCtrl;
    case 30: return ImGuiKey::ImGuiKey_A;
    case 31: return ImGuiKey::ImGuiKey_S;
    case 32: return ImGuiKey::ImGuiKey_D;
    case 33: return ImGuiKey::ImGuiKey_F;
    case 34: return ImGuiKey::ImGuiKey_G;
    case 35: return ImGuiKey::ImGuiKey_H;
    case 36: return ImGuiKey::ImGuiKey_J;
    case 37: return ImGuiKey::ImGuiKey_K;
    case 38: return ImGuiKey::ImGuiKey_L;
    case 39: return ImGuiKey::ImGuiKey_Semicolon;
    case 40: return ImGuiKey::ImGuiKey_Apostrophe;
    case 41: return ImGuiKey::ImGuiKey_GraveAccent;
    case 42: return ImGuiKey::ImGuiKey_LeftShift;
    case 43: return ImGuiKey::ImGuiKey_Backslash;
    case 44: return ImGuiKey::ImGuiKey_Z;
    case 45: return ImGuiKey::ImGuiKey_X;
    case 46: return ImGuiKey::ImGuiKey_C;
    case 47: return ImGuiKey::ImGuiKey_V;
    case 48: return ImGuiKey::ImGuiKey_B;
    case 49: return ImGuiKey::ImGuiKey_N;
    case 50: return ImGuiKey::ImGuiKey_M;
    case 51: return ImGuiKey::ImGuiKey_Comma;
    case 52: return ImGuiKey::ImGuiKey_Period;
    case 53: return ImGuiKey::ImGuiKey_Slash;
    case 54: return ImGuiKey::ImGuiKey_RightShift;
    case 55: return ImGuiKey::ImGuiKey_KeypadMultiply;
    case 56: return ImGuiKey::ImGuiKey_LeftAlt;
    case 57: return ImGuiKey::ImGuiKey_Space;
    case 58: return ImGuiKey::ImGuiKey_CapsLock;
    case 59: return ImGuiKey::ImGuiKey_F1;
    case 60: return ImGuiKey::ImGuiKey_F2;
    case 61: return ImGuiKey::ImGuiKey_F3;
    case 62: return ImGuiKey::ImGuiKey_F4;
    case 63: return ImGuiKey::ImGuiKey_F5;
    case 64: return ImGuiKey::ImGuiKey_F6;
    case 65: return ImGuiKey::ImGuiKey_F7;
    case 66: return ImGuiKey::ImGuiKey_F8;
    case 67: return ImGuiKey::ImGuiKey_F9;
    case 68: return ImGuiKey::ImGuiKey_F10;
    case 69: return ImGuiKey::ImGuiKey_Pause;
    case 70: return ImGuiKey::ImGuiKey_ScrollLock;
    case 71: return ImGuiKey::ImGuiKey_Keypad7;
    case 72: return ImGuiKey::ImGuiKey_Keypad8;
    case 73: return ImGuiKey::ImGuiKey_Keypad9;
    case 74: return ImGuiKey::ImGuiKey_KeypadSubtract;
    case 75: return ImGuiKey::ImGuiKey_Keypad4;
    case 76: return ImGuiKey::ImGuiKey_Keypad5;
    case 77: return ImGuiKey::ImGuiKey_Keypad6;
    case 78: return ImGuiKey::ImGuiKey_KeypadAdd;
    case 79: return ImGuiKey::ImGuiKey_Keypad1;
    case 80: return ImGuiKey::ImGuiKey_Keypad2;
    case 81: return ImGuiKey::ImGuiKey_Keypad3;
    case 82: return ImGuiKey::ImGuiKey_Keypad0;
    case 83: return ImGuiKey::ImGuiKey_KeypadDecimal;
        //case 86: return ImGuiKey_::KeyPipe;
    case 87: return ImGuiKey::ImGuiKey_F11;
    case 88: return ImGuiKey::ImGuiKey_F12;
    case 284: return ImGuiKey::ImGuiKey_KeypadEnter;
    case 285: return ImGuiKey::ImGuiKey_RightCtrl;
    case 309: return ImGuiKey::ImGuiKey_KeypadDivide;
    case 311: return ImGuiKey::ImGuiKey_PrintScreen;
    case 312: return ImGuiKey::ImGuiKey_RightAlt;
    case 325: return ImGuiKey::ImGuiKey_NumLock;
    case 328: return ImGuiKey::ImGuiKey_UpArrow;
    case 329: return ImGuiKey::ImGuiKey_PageUp;
    case 331: return ImGuiKey::ImGuiKey_LeftArrow;
    case 333: return ImGuiKey::ImGuiKey_RightArrow;
    case 335: return ImGuiKey::ImGuiKey_End;
    case 336: return ImGuiKey::ImGuiKey_DownArrow;
    case 337: return ImGuiKey::ImGuiKey_PageDown;
    case 338: return ImGuiKey::ImGuiKey_Insert;
    case 339: return ImGuiKey::ImGuiKey_Delete;
    case 347: return ImGuiKey::ImGuiKey_LeftSuper;
    case 348: return ImGuiKey::ImGuiKey_RightSuper;
    case 349: return ImGuiKey::ImGuiKey_Menu;
    default: return ImGuiKey::ImGuiKey_None;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
