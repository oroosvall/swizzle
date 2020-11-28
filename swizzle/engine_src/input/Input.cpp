
#include "swizzle/input/Input.hpp"
#include "swizzle/Swizzle.hpp"

#include <unordered_map>


namespace swizzle
{
    namespace input
    {
		class InputCallback : public EventHandler<core::WindowEvent>
		{
		public:
			virtual void publishEvent(const core::WindowEvent& evt) override;
		};

		static struct
		{
			core::Resource<core::Window> mActiveWindow;
			InputCallback mCb;
			float mDx;
			float mDy;
			int32_t mX;
			int32_t mY;
			std::unordered_map<Keys, bool> mPressedKeys;
		} inputCtx;

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
		}

		bool IsKeyPressed(const Keys key)
		{
			bool pressed = false;
			if (inputCtx.mPressedKeys.count(key) != 0u)
			{
				pressed = inputCtx.mPressedKeys[key];
			}
			return pressed;
		}

		bool IsActionPressed(const KeyAction& action)
		{
			return false;
		}

		void GetMousePosition(int32_t& xPos, int32_t& yPos)
		{
			xPos = inputCtx.mX;
			yPos = inputCtx.mY;
		}

		void GetMouseDelta(float& x, float& y)
		{
			x = inputCtx.mDx;
			y = inputCtx.mDy;
		}

		void InputCallback::publishEvent(const core::WindowEvent& evt)
		{
			core::WindowEventType evtType = evt.getEventType();
			
			switch (evtType)
			{
			case core::WindowEventType::FocusEvent:
				break;
			case core::WindowEventType::KeyboardInputEvent:
			{
				core::InputEvent& e = (core::InputEvent&)evt;

				if (swizzle::core::KeyToScanCode(Keys::KeyNone) == e.mKey)
				{
					LOG_INFO("Key %d was None", e.mKey);
				}
				else
				{
					if (e.mFromKeyboard)
					{
						printf("%s, ", sw::core::GetKeyText(e.mKey));
						wprintf(L"%s\n", sw::core::GetKeyTextW(e.mKey));
					}
				}
				
				// printf("%d, %c\n", e.mKey, MapVirtualKeyExA(e.mKey, MAPVK_VSC_TO_VK | MAPVK_VK_TO_CHAR, GetKeyboardLayout(0)));
				break;
			}
			case core::WindowEventType::CharacterTypeEvent:
				break;
			case core::WindowEventType::MouseMoveEvent:
			{
				core::MouseMoveEvent& e = (core::MouseMoveEvent&)evt;
				inputCtx.mX = e.mX;
				inputCtx.mY = e.mY;
				break;
			}
			case core::WindowEventType::MouseMoveDeltaEvent:
			{
				core::MouseMoveDelta &e = (core::MouseMoveDelta&)evt;
				inputCtx.mDx = (float)e.dX;
				inputCtx.mDy = (float)e.dY;
				break;
			}
			case core::WindowEventType::MouseScrollEvent:
				break;
			default:
				break;
			}

		}

    }
}