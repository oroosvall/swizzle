#ifdef SW_WINDOWS

#include "PlatformLayer.hpp"
#include <swizzle/Platform.hpp>

#include "Windows/Win32Window.hpp"

#include "gfxVulkan/VulkanRenderer.hpp"
#include "gfxOpengl/GlRenderer.hpp"

#include "../Logging/Logger.hpp"

namespace swizzle
{
	LARGE_INTEGER gFrequency;
	
	void platformInit()
	{
		LOG_INFO("platformInit() Windows");
		WNDCLASS cls;

		cls.style = CS_HREDRAW | CS_VREDRAW;
		cls.lpfnWndProc = WindowProc;
		cls.cbClsExtra = 0;
		cls.cbWndExtra = 0;
		cls.hInstance = GetModuleHandle(NULL);
		cls.hIcon = NULL;
		cls.hCursor = LoadCursor(NULL, IDC_ARROW);
		cls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		cls.lpszMenuName = NULL;
		cls.lpszClassName = gWindowClass;

		QueryPerformanceFrequency(&gFrequency);

		if (RegisterClass(&cls) == 0)
		{
			LOG_ERROR("Failed to register window class");
		}

	}

	void platformDeInit()
	{
		if (UnregisterClass(gWindowClass, GetModuleHandle(NULL)) == 0)
		{
			LOG_ERROR("Failed to free window class");
		}
	}

	uint32_t getPlatformDisplayCount()
	{
		return 1;
	}

	Resolution getPlatformDisplayResolution(uint32_t displayIndex)
	{
		displayIndex;
		return Resolution();
	}

	Window* createPlatformWindow(uint32_t width, uint32_t height, const char* title)
	{
		return new Win32Window(width, height, title);
	}

	Renderer* platformGfxVulkan(Window* window)
	{
		VulkanRenderer* vkRend = new VulkanRenderer(window);
		
		return vkRend;
	}

	Renderer* platformGfxOpenGl(Window* window)
	{
		window;
		return nullptr;
	}

	namespace platform
	{
		uint64_t getPlatformTimeStamp()
		{
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);

			time.QuadPart *= 1000000U;
			time.QuadPart /= gFrequency.QuadPart;

			return static_cast<uint64_t>(time.QuadPart);
		}

		const uint32_t getPlatformCurrentThreadId()
		{
			const DWORD threadId = GetCurrentThreadId();
			return static_cast<const uint32_t>(threadId);
		}
	}

}

#endif