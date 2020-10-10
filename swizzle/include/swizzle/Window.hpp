#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <cstdint>

#include "events/event.hpp"
#include "events/windowEvents.hpp"

namespace swizzle
{

	struct Resolution
	{
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mFrequency;
	};

	class Window
	{
	public:

		virtual ~Window() {}

		virtual void show() = 0;
		virtual void hide() = 0;

		virtual void addEventListener(EventHandler<WindowEvent>* listener) = 0;
		virtual void removeEventListener(EventHandler<WindowEvent>* listener) = 0;

		virtual void setTitle(const char* title) = 0;
		virtual void setTitle(const wchar_t* title) = 0;

		virtual void setSize(const uint32_t width, uint32_t height) = 0;

		virtual bool isVisible() const = 0;
		
		virtual void setBorderless(bool borderless) = 0;

		virtual void getCursorPos(uint32_t& xPos, uint32_t& yPos) const = 0;
		virtual void getSize(uint32_t& width, uint32_t& height) const = 0;
		virtual void* getNativeHandle() const = 0;

		virtual void setCursorVisible(bool visible) = 0;
		virtual bool isCursorVisible() const = 0;

		virtual void pollEvents() = 0;

	};
}

#endif