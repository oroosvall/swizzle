#ifdef SW_WINDOWS

#include "Win32Input.hpp"

#include <unordered_map>
#include <Windows.h>

namespace swizzle
{
    namespace core
    {
        //static const input::Keys keyMapping[] =
        //{
        //    /* 0  */ input::Keys::KeyNone,
        //    /* 1  */ input::Keys::KeyEsc,
        //    /* 2  */ input::Keys::Key0,
        //    /* 3  */ input::Keys::Key1,
        //    /* 4  */ input::Keys::Key2,
        //    /* 5  */ input::Keys::Key3,
        //    /* 6  */ input::Keys::Key4,
        //    /* 7  */ input::Keys::Key5,
        //    /* 8  */ input::Keys::Key6,
        //    /* 9  */ input::Keys::Key7,
        //    /* 10 */ input::Keys::Key8,
        //    /* 11 */ input::Keys::Key9,
        //    /* 12 */ input::Keys::KeyHyphen,
        //    /* 13 */ input::Keys::KeyEquals,
        //    /* 14 */ input::Keys::KeyBackspace,
        //    /* 15 */ input::Keys::KeyTab,
        //    /* 16 */ input::Keys::KeyQ,
        //    /* 17 */ input::Keys::KeyW,
        //    /* 18 */ input::Keys::KeyE,
        //    /* 19 */ input::Keys::KeyR,
        //    /* 20 */ input::Keys::KeyT,
        //    /* 21 */ input::Keys::KeyY,
        //    /* 22 */ input::Keys::KeyU,
        //    /* 23 */ input::Keys::KeyI,
        //    /* 24 */ input::Keys::KeyO,
        //    /* 25 */ input::Keys::KeyP,
        //    /* 26 */ input::Keys::KeyLBracket,
        //    /* 27 */ input::Keys::KeyRBracket,
        //    /* 28 */ input::Keys::KeyEnter,
        //    /* 29 */ input::Keys::KeyLCtrl,
        //    /* 30 */ input::Keys::KeyA,
        //    /* 31 */ input::Keys::KeyS,
        //    /* 32 */ input::Keys::KeyD,
        //    /* 33 */ input::Keys::KeyF,
        //    /* 34 */ input::Keys::KeyG,
        //    /* 35 */ input::Keys::KeyH,
        //    /* 36 */ input::Keys::KeyJ,
        //    /* 37 */ input::Keys::KeyK,
        //    /* 38 */ input::Keys::KeyL,
        //    /* 39 */ input::Keys::KeySemiColon,
        //    /* 40 */ input::Keys::KeyApostrophe,
        //    /* 41 */ input::Keys::KeyTilde,
        //    /* 42 */ input::Keys::KeyLShift,
        //    /* 43 */ input::Keys::KeyBackslash,
        //    /* 44 */ input::Keys::KeyZ,
        //    /* 45 */ input::Keys::KeyX,
        //    /* 46 */ input::Keys::KeyC,
        //    /* 47 */ input::Keys::KeyV,
        //    /* 48 */ input::Keys::KeyB,
        //    /* 49 */ input::Keys::KeyN,
        //    /* 50 */ input::Keys::KeyM,
        //    /* 51 */ input::Keys::KeyComma,
        //    /* 52 */ input::Keys::KeyPeriod,
        //    /* 53 */ input::Keys::KeyForwardslash,
        //    /* 54 */ input::Keys::KeyRShift,
        //    /* 55 */ input::Keys::KeyNumPadAsterisk,
        //    /* 56 */ input::Keys::KeyLAlt,
        //    /* 57 */ input::Keys::KeySpace,
        //    /* 58 */ input::Keys::KeyCaps,
        //    /* 59 */ input::Keys::KeyF1,
        //    /* 60 */ input::Keys::KeyF2,
        //    /* 61 */ input::Keys::KeyF3,
        //    /* 62 */ input::Keys::KeyF4,
        //    /* 63 */ input::Keys::KeyF5,
        //    /* 64 */ input::Keys::KeyF6,
        //    /* 65 */ input::Keys::KeyF7,
        //    /* 66 */ input::Keys::KeyF8,
        //    /* 67 */ input::Keys::KeyF9,
        //    /* 68 */ input::Keys::KeyF10,
        //    /* 69 */ input::Keys::KeyPause,
        //    /* 70 */ input::Keys::KeyScrollLock,
        //    /* 71 */ input::Keys::KeyNumPad7,
        //    /* 72 */ input::Keys::KeyNumPad8,
        //    /* 73 */ input::Keys::KeyNumPad9,
        //    /* 74 */ input::Keys::KeyNumPadHyphon,
        //    /* 75 */ input::Keys::KeyNumPad4,
        //    /* 76 */ input::Keys::KeyNumPad5,
        //    /* 77 */ input::Keys::KeyNumPad6,
        //    /* 78 */ input::Keys::KeyNumPadPlus,
        //    /* 79 */ input::Keys::KeyNumPad1,
        //    /* 80 */ input::Keys::KeyNumPad2,
        //    /* 81 */ input::Keys::KeyNumPad3,
        //    /* 82 */ input::Keys::KeyNumPad0,
        //    /* 83 */ input::Keys::KeyNumPadDelete,
        //    /* 84 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 85 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 86 */ input::Keys::KeyPipe,
        //    /* 87 */ input::Keys::KeyF11,
        //    /* 88 */ input::Keys::KeyF12,
        //};

        //// Extended keys are all keys above 255
        //static const input::Keys keyMappingExtended[] =
        //{
        //    /* 256 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 257 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 258 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 259 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 260 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 261 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 262 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 263 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 264 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 265 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 266 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 267 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 268 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 269 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 270 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 271 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 272 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 273 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 274 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 275 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 276 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 277 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 278 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 279 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 280 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 281 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 282 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 283 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 284 */ input::Keys::KeyNumPadEnter,
        //    /* 285 */ input::Keys::KeyRCtrl,
        //    /* 286 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 287 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 288 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 289 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 290 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 291 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 292 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 293 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 294 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 295 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 296 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 297 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 298 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 299 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 300 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 301 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 302 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 303 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 304 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 305 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 306 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 307 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 308 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 309 */ input::Keys::KeyNumPadSlash,
        //    /* 310 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 311 */ input::Keys::KeySysReq,
        //    /* 312 */ input::Keys::KeyRAlt,
        //    /* 313 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 314 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 315 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 316 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 317 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 318 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 319 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 320 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 321 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 322 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 323 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 324 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 325 */ input::Keys::KeyNumPad,
        //    /* 326 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 327 */ input::Keys::KeyHome,
        //    /* 328 */ input::Keys::KeyArrowUp,
        //    /* 329 */ input::Keys::KeyPageUp,
        //    /* 330 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 331 */ input::Keys::KeyArrowLeft,
        //    /* 332 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 333 */ input::Keys::KeyArrowRight,
        //    /* 334 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 335 */ input::Keys::KeyEnd,
        //    /* 336 */ input::Keys::KeyArrowDown,
        //    /* 337 */ input::Keys::KeyPageDown,
        //    /* 338 */ input::Keys::KeyInsert,
        //    /* 339 */ input::Keys::KeyDelete,
        //    /* 340 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 341 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 342 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 343 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 344 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 345 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 346 */ input::Keys::KeyNone, // This key is not mapped
        //    /* 347 */ input::Keys::KeyLSpecial,
        //    /* 348 */ input::Keys::KeyRSpecial,
        //    /* 349 */ input::Keys::KeyApplication,
        //};

        //const input::Keys ScanCode2Key(S32 scanCode)
        //{
        //    input::Keys key = input::Keys::KeyNone;
        //    S32 maxSize = _countof(keyMapping);
        //    if (scanCode < maxSize)
        //    {
        //        key = keyMapping[scanCode];
        //    }
        //    maxSize = _countof(keyMappingExtended);
        //    if ((scanCode & 0x100) != 0)
        //    {
        //        scanCode &= 0xFF;
        //        if ((scanCode & 0xFF) < maxSize)
        //        {
        //            key = keyMappingExtended[scanCode];
        //        }
        //    }
        //    return key;
        //}

        static const S32 scanCodeMapping[] =
        {
            /* KeyEsc,                                                      */ 1,
            /*                                                              */
            /* Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,  */ 11, 2, 3, 4, 5, 6, 7, 8, 9, 10,
            /*                                                              */
            /* KeyHyphen(-), KeyEquals (=),                                 */ 12, 13,
            /*                                                              */
            /* KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ,  */ 30, 48, 46, 32, 18, 33, 34, 35, 23, 36,
            /* KeyK, KeyL, KeyM, KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS, KeyT,  */ 37, 38, 50, 49, 24, 25, 16, 19, 31, 20,
            /* KeyU, KeyV, KeyW, KeyX, KeyY, KeyZ,                          */ 22, 47, 17, 45, 21, 44,
            /*                                                              */
            /* KeyLBracket, KeyRBracket, KeyEnter,                          */ 26, 27, 28,
            /* KeySemiColon, KeyApostrophe, KeyBackslash, KeyForwardslash,  */ 39, 40, 43, 53,
            /*                                                              */
            /* KeySpace, KeyTilde, KeyPause, KeyScrollLock,                 */ 57, 41, 69, 70,
            /* KeySysReq, KeyNumPad, KeyApplication,                        */ 311, 325, 349,
            /*                                                              */
            /* KeyArrowUp, KeyArrowDown, KeyArrowLeft, KeyArrowRight,       */ 328, 336, 331, 333,
            /*                                                              */
            /* KeyF1, KeyF2, KeyF3, KeyF4, KeyF5, KeyF6,                    */ 59, 60, 61, 62, 63, 64,
            /* KeyF7, KeyF8, KeyF9, KeyF10, KeyF11, KeyF12,                 */ 65, 66, 67, 68, 87, 88,
            /*                                                              */
            /* KeyLCtrl, KeyRCtrl,                                          */ 29, 285,
            /* KeyLShift, KeyRShift,                                        */ 42, 54,
            /* KeyLAlt, KeyRAlt,                                            */ 56, 312,
            /* KeyLSpecial, KeyRSpecial,                                    */ 347, 348,
            /*                                                              */
            /* KeyTab, KeyCaps, KeyPipe,                                    */ 15, 58, 86,
            /* KeyComma, KeyPeriod,                                         */ 51, 52,
            /*                                                              */
            /* KeyNumPad0, KeyNumPad1, KeyNumPad2,                          */ 82, 79, 80,
            /* KeyNumPad3, KeyNumPad4, KeyNumPad5,                          */ 81, 75, 76,
            /* KeyNumPad6, KeyNumPad7, KeyNumPad8,                          */ 77, 71, 72,
            /* KeyNumPad9,                                                  */ 73,
            /*                                                              */
            /* KeyDelete, KeyInsert, KeyHome,                               */ 339, 338, 327,
            /* KeyEnd, KeyPageUp, KeyPageDown,                              */ 335, 329, 337,
            /*                                                              */
            /* KeyBackspace,                                                */ 14,
            /* KeyNumPadEnter,                                              */ 284,
            /* KeyNumPadPlus,                                               */ 78,
            /* KeyNumPadHyphon,                                             */ 74,
            /* KeyNumPadAsterisk,                                           */ 55,
            /* KeyNumPadSlash,                                              */ 309,
            /* KeyNumPadDelete,                                             */ 83,
        };

        S32 Key2ScanCode(input::Keys key)
        {
            S32 scanCode = 0;
            S32 maxSize = _countof(scanCodeMapping);
            if ((S32)key < maxSize)
            {
                scanCode = scanCodeMapping[(S32)key];
            }

            return scanCode;
        }

        static std::unordered_map<S32, const SwChar*> keyTextMap;
        static std::unordered_map<S32, const SwWChar*> keyTextMapW;

        void InputInit()
        {
            for (S32 i = 0; i < 512; ++i)
            {
                S32 keyy = (i << 16);
                SwChar* buf = new SwChar[50];
                SwWChar* bufw = new SwWChar[50];
                memset(buf, 0, 50);
                GetKeyNameTextA((LONG)keyy, buf, 50);
                GetKeyNameTextW((LONG)keyy, bufw, 50);
                keyTextMap[i] = buf;
                keyTextMapW[i] = bufw;
            }
        }

        void InputCleanup()
        {
            for (auto& elem : keyTextMap)
            {
                delete[] elem.second;
            }
            keyTextMap.clear();

            for (auto& elem : keyTextMapW)
            {
                delete[] elem.second;
            }
            keyTextMapW.clear();
        }

        const SwChar* GetWinKeyText(S32 scanCode)
        {
            const SwChar* ret = "";
            if (scanCode < 512)
            {
                ret = keyTextMap[scanCode];
            }

            return ret;
        }

        const SwWChar* GetWinKeyTextW(S32 scanCode)
        {
            const SwWChar* ret = L"";
            if (scanCode < 512)
            {
                ret = keyTextMapW[scanCode];
            }

            return ret;
        }
    }
}

#endif