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

	BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
	{
		hMonitor;
		hdcMonitor;
		lprcMonitor;
		int* count = (int*)dwData;
		(*count)++;
		return TRUE;
	}

	uint32_t getPlatformDisplayCount()
	{
		uint32_t count = 0U;

		EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&count);

		return count;
	}

	void getPlatformDisplayResolution(uint32_t displayIndex, uint32_t& resolutionCount, Resolution* resolutions)
	{
		displayIndex;
		uint32_t count = 0U;

		DEVMODE deviceMode = { 0U };
		deviceMode.dmSize = sizeof(DEVMODE);
		deviceMode.dmDriverExtra = 0;

		if (resolutions == nullptr)
		{
			while(EnumDisplaySettings(NULL, count, &deviceMode) != 0)
			{
				count++;
			}
			resolutionCount = count;
		}
		else
		{
			while (EnumDisplaySettings(NULL, count, &deviceMode) != 0 && count != resolutionCount)
			{
				resolutions[count].mHeight = deviceMode.dmPelsHeight;
				resolutions[count].mWidth = deviceMode.dmPelsWidth;
				resolutions[count].mFrequency = deviceMode.dmDisplayFrequency;
				count++;
			}
		}

	}

	Resource<Window> createPlatformWindow(uint32_t width, uint32_t height, const char* title)
	{
		return std::make_shared<Win32Window>(width, height, title);
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