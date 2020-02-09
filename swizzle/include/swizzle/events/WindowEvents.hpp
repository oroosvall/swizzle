#ifndef WINDOW_EVENTS_HPP
#define WINDOW_EVENTS_HPP

#include <swizzle/Api.hpp>
#include <cstdint>

namespace swizzle
{

	enum class eWindowEventType
	{
		eType_resize,
		eType_focus,
		eType_close,
		eType_input,
		eType_type,
		eType_move,
		eType_moveDelta,
		eType_scroll
	};

#define MODKEY_SHIFT    0x01
#define MODKEY_CONTROL  0x02
#define MODKEY_ALT      0x04
#define MODKEY_SUPER    0x08

	class WindowEvent
	{
	public:
		virtual const eWindowEventType getEventType() const = 0;

	};

	class ResizeEvent : public WindowEvent
	{
	public:
		virtual const eWindowEventType getEventType() const override { return eWindowEventType::eType_resize; }
		int32_t mWidth;
		int32_t mHeight;
	};

	class FocusEvent : public WindowEvent
	{
	public:
		virtual const eWindowEventType getEventType() const override { return eWindowEventType::eType_focus; }
		bool mFocused;
	};

	class CloseEvent : public WindowEvent
	{
	public:
		virtual const eWindowEventType getEventType() const override { return eWindowEventType::eType_close; }
	};

	class InputEvent : public WindowEvent
	{
	public:
		virtual const eWindowEventType getEventType() const override { return eWindowEventType::eType_input; }

		bool mFromKeyboard;
		int32_t mKey;
		int32_t mModKeys;
		bool mPressed;
	};

	class CharacterEvent : public WindowEvent
	{
	public:
		virtual const eWindowEventType getEventType() const override { return eWindowEventType::eType_type; }
		uint32_t mCodePoint;
	};

	class MoveEvent : public WindowEvent
	{
	public:
		virtual const eWindowEventType getEventType() const override { return eWindowEventType::eType_move; }
		
		int32_t mX;
		int32_t mY;
	};

	class MoveDelta : public WindowEvent
	{
	public:
		virtual const eWindowEventType getEventType() const override { return eWindowEventType::eType_moveDelta; }

		int32_t dX;
		int32_t dY;
	};

	class ScrollEvent : public WindowEvent
	{
	public:
		virtual const eWindowEventType getEventType() const override { return eWindowEventType::eType_scroll; }

		int32_t mScrollX;
		int32_t mScrollY;
	};

}

#endif