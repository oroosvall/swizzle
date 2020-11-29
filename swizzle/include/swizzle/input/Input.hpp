#ifndef INPUT_HPP
#define INPUT_HPP

#include "swizzle/Api.hpp"
#include "swizzle/core/platform/Window.hpp"
#include "swizzle/core/common/Resource.hpp"

#include <cstdint>

namespace swizzle
{
	namespace input
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
			Button3,
			Button4,
		};

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

		void SWIZZLE_API SetInputSource(core::Resource<core::Window> window);
		
		bool SWIZZLE_API RegisterKeyAction(const KeyAction& action);

		void SWIZZLE_API InputFrameReset();

		bool SWIZZLE_API IsKeyPressed(const Keys key);
		bool SWIZZLE_API IsActionPressed(const KeyAction& action);

		bool SWIZZLE_API IsMouseButtonPressed(const Mouse key);

		void SWIZZLE_API GetMousePosition(int32_t& xPos, int32_t& yPos);
		void SWIZZLE_API GetMouseDelta(float& x, float& y);

	}
}


#endif