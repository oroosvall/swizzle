#ifndef WIN32WINDOW_HPP
#define WIN32WINDOW_HPP 

#ifdef SW_WINDOWS

#include <swizzle/Window.hpp>

#include <Windows.h>

#include <EventSystem/EventList.hpp>

namespace swizzle
{

	const LPWSTR gWindowClass = L"SwizzleWindowClass";
	LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	class Win32Window : public Window
	{
	public:
		Win32Window(uint32_t width, uint32_t height, const char* title);
		virtual ~Win32Window();

		virtual void show() override;
		virtual void hide() override;
		
		virtual void addEventListener(EventHandler<WindowEvent>* listener) override;
		virtual void removeEventListener(EventHandler<WindowEvent>* listener) override;

		virtual void setTitle(const char* title) override;
		virtual void setTitle(const wchar_t* title) override;

		virtual void setSize(const uint32_t width, uint32_t height) override;

		virtual bool isVisible() const override;

		virtual void getCursorPos(uint32_t& xPos, uint32_t& yPos) const override;
		virtual void getSize(uint32_t& width, uint32_t& height) const override;
		virtual void* getNativeHandle() const override;
		virtual void pollEvents() override;

		virtual void setCursorVisible(bool visible) override;
		virtual bool isCursorVisible() const override;

		EventHandlerList<WindowEvent>& getEventHandler();

		int32_t modKeys;

	private:
		HWND mWnd;
		bool mCursorVisible;

		EventHandlerList<WindowEvent> mEventHandlers;

	};

}

#endif

#endif