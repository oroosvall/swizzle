#ifndef PLATFORM_LAYER_HPP
#define PLATFORM_LAYER_HPP

#include <swizzle/Window.hpp>
#include <swizzle/Renderer.hpp>

namespace swizzle
{

	void platformInit();
	void platformDeInit();

	uint32_t getPlatformDisplayCount();

	Resolution getPlatformDisplayResolution(uint32_t displayIndex);

	Window* createPlatformWindow(uint32_t width, uint32_t height, const char* title);

	Renderer* platformGfxVulkan(Window* window);
	Renderer* platformGfxOpenGl(Window* window);

}

#endif