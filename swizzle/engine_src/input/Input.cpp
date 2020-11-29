
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
			std::unordered_map<S32, SwBool> mPressedKeys;
			std::unordered_map<S32, SwBool> mPressedMouse;
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
			return pressed;
		}

		bool IsActionPressed(const KeyAction& action)
		{
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
			return pressed;
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