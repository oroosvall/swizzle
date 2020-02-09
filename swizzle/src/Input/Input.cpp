#include <swizzle/input/Input.hpp>

#include <unordered_map>

namespace swizzle
{
	namespace input
	{

		class InputEventListener : public swizzle::EventHandler<swizzle::WindowEvent>
		{
		public:
			InputEventListener()
				: mHasFocus(false)
				, mIsPressed()
				, mWasPressedThisFrame()
				, mWasReleasedThisFrame()
				, ox(0), oy(0)
				, x(0), y(0)
				, dx(0.0F), dy(0.0F)
			{
				mIsPressed[Keys::eKey_None] = false;
			}


			void translateKey(int32_t keyCode, bool pressed)
			{
				switch (keyCode)
				{
				case 17:
					mIsPressed[Keys::eKey_W] = pressed;
					break;
				case 30:
					mIsPressed[Keys::eKey_A] = pressed;
					break;
				case 31:
					mIsPressed[Keys::eKey_S] = pressed;
					break;
				case 32:
					mIsPressed[Keys::eKey_D] = pressed;
					break;
				default:
					break;
				}
			}

			void translateModKeys(int32_t modkeys)
			{
				mIsPressed[Keys::eKey_Shift] = ((modkeys & MODKEY_SHIFT) == MODKEY_SHIFT);
				mIsPressed[Keys::eKey_Ctrl] = ((modkeys & MODKEY_CONTROL) == MODKEY_CONTROL);
				mIsPressed[Keys::eKey_Alt] = ((modkeys & MODKEY_ALT) == MODKEY_ALT);
				mIsPressed[Keys::eKey_Special] = ((modkeys & MODKEY_SUPER) == MODKEY_SUPER);	
			}

			void publishEvent(const swizzle::WindowEvent& evt)
			{
				switch (evt.getEventType())
				{
				case swizzle::eWindowEventType::eType_focus:
					mHasFocus = ((swizzle::FocusEvent&)evt).mFocused;
					break;
				case swizzle::eWindowEventType::eType_input:
				{
					const swizzle::InputEvent& e = (swizzle::InputEvent&)evt;
					
					translateKey(e.mKey, e.mPressed);
					translateModKeys(e.mModKeys);
				}
					break;
				case swizzle::eWindowEventType::eType_move:
				{
					const swizzle::MoveEvent& e = (swizzle::MoveEvent&)evt;
					x = e.mX;
					y = e.mY;
				}
					break;
				case swizzle::eWindowEventType::eType_moveDelta:
				{
					const swizzle::MoveDelta& e = (swizzle::MoveDelta&)evt;
					dx = float(e.dX);
					dy = float(e.dY);
				}
				break;
				case swizzle::eWindowEventType::eType_scroll:
				default:
					break;
				}
			}

			bool mHasFocus = false;

			std::unordered_map<Keys, bool> mIsPressed;
			std::unordered_map<Keys, bool> mWasPressedThisFrame;
			std::unordered_map<Keys, bool> mWasReleasedThisFrame;

			uint32_t ox, oy;
			uint32_t x, y;

			float dx, dy;

			bool isKeyPressed(const Keys key)
			{
				bool success = false;
				if (mHasFocus)
				{
					success = mIsPressed[key];
				}
				return success;
			}

			bool isActionPressed(const KeyAction& action)
			{
				action;
				return false;
			}

		};

		static Window* gInputWindow = nullptr;
		static InputEventListener gInputListener;



		void setInputSource(Window* window)
		{
			if (gInputWindow != nullptr)
			{
				gInputWindow->removeEventListener(&gInputListener);
			}
			gInputWindow = nullptr;
			if (window != nullptr)
			{
				gInputWindow = window;
				gInputWindow->addEventListener(&gInputListener);
			}
		}

		void inputFrameReset()
		{
			gInputListener.mWasPressedThisFrame.clear();
			gInputListener.mWasReleasedThisFrame.clear();

			gInputListener.dx = 0.0F;
			gInputListener.dy = 0.0F;

			gInputListener.ox = gInputListener.x;
			gInputListener.oy = gInputListener.y;
		}

		bool registerKeyAction(const KeyAction& action)
		{
			action;
			return false;
		}

		bool isKeyPressed(const Keys key)
		{
			return gInputListener.isKeyPressed(key);
		}

		bool isActionPressed(const KeyAction& action)
		{
			return gInputListener.isActionPressed(action);
		}

		void getMousePosition(int32_t& xPos, int32_t& yPos)
		{
			if (gInputListener.mHasFocus)
			{
				xPos = gInputListener.x;
				yPos = gInputListener.y;
			}
		}

		void getMouseDelta(float& x, float& y)
		{
			if (gInputListener.mHasFocus)
			{
				x = gInputListener.dx;
				y = gInputListener.dy;
			}
		}

	}
}