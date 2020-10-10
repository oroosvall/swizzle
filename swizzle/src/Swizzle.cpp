
#include <swizzle/Swizzle.hpp>

#include "Platform/PlatformLayer.hpp"

#include "Logging/Logger.hpp"

#include "Debugging/Profile.hpp"

namespace swizzle
{

	void SwInitialize()
	{
		START_PROFILER("test", "");
		platformInit();
	}

	void SwCleanup()
	{
		END_PROFILER();
		platformDeInit();
	}

	void SwAddLogger(LoggerIfc* logger)
	{
		if (logger != nullptr)
		{
			Logger::getLogger().addLogger(logger);
		}
	}

	uint32_t SwGetNumDisplays()
	{
		return getPlatformDisplayCount();
	}

	void SWIZZLE_API SwGetDisplayResolution(uint32_t displayIndex, uint32_t& resolutionCount, Resolution* resolutions)
	{
		getPlatformDisplayResolution(displayIndex, resolutionCount, resolutions);
	}
	

	Resource<Window> SwCreateWindow(uint32_t width, uint32_t height, const char* title)
	{
		return Resource<Window>(createPlatformWindow(width, height, title));
	}

	Resource<Renderer> SwCreateRenderer(Window* window, eRendererBackend backend)
	{
		PROFILE_FUNCTION();
		Renderer* rend = nullptr;

		switch (backend)
		{
		case swizzle::eRendererBackend::eRendererVulkan:
			rend = platformGfxVulkan(window);
			break;
		case swizzle::eRendererBackend::eRendererOpengl:
			rend = platformGfxOpenGl(window);
			break;
		default:
		{
			LOG_ERROR("Invalid graphics backend");
		}
		break;
		}

		return Resource<Renderer>(rend);
	}

}