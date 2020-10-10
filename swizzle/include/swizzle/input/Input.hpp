#ifndef INPUT_HPP
#define INPUT_HPP

#include "../Api.hpp"
#include "../Window.hpp"

#include <cstdint>

namespace swizzle
{
	namespace input
	{

		enum class Keys
		{
			eKey_None = -1,
			eKey_W,
			eKey_A,
			eKey_S,
			eKey_D,

			eKey_Ctrl,
			eKey_Shift,
			eKey_Alt,
			eKey_Special,
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

		void SWIZZLE_API setInputSource(Window* window);
		
		bool SWIZZLE_API registerKeyAction(const KeyAction& action);

		void SWIZZLE_API inputFrameReset();

		bool SWIZZLE_API isKeyPressed(const Keys key);
		bool SWIZZLE_API isActionPressed(const KeyAction& action);

		void SWIZZLE_API getMousePosition(int32_t& xPos, int32_t& yPos);
		void SWIZZLE_API getMouseDelta(float& x, float& y);

	}
}


#endif