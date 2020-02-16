#include "Api.hpp"

#include "Window.hpp"
#include "Renderer.hpp"

#include "gfxutils/Vectors.hpp"
#include "gfxutils/Matrix.hpp"

#include "gfxutils/Camera.hpp"

#include "Logging.hpp"

#include "gfxutils/MeshLoader.hpp"

#include <swizzle/Script.hpp>

#include "input/Input.hpp"

#include <cstdint>

namespace swizzle
{

	void SWIZZLE_API SwInitialize();
	void SWIZZLE_API SwCleanup();

	void SWIZZLE_API SwAddLogger(LoggerIfc* logger);

	uint32_t SWIZZLE_API SwGetNumDisplays();
	void SWIZZLE_API SwGetDisplayResolution(uint32_t displayIndex, uint32_t& resolutionCount, Resolution* resolutions);

	Resource<Window> SWIZZLE_API SwCreateWindow(uint32_t width, uint32_t height, const char* title);
	Resource<Renderer> SWIZZLE_API SwCreateRenderer(Window* window, eRendererBackend backend);

}

namespace sw = swizzle;