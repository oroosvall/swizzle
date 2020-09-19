#ifndef SWIZZLE_HPP
#define SWIZZLE_HPP

#include <swizzle/core/common/Types.hpp>
#include <swizzle/Api.hpp>

#include <swizzle/core/logging/Logging.hpp>
#include <swizzle/core/platform/Platform.hpp>

#include <swizzle/gfx/Gfx.hpp>

namespace swizzle
{
    SwBool SWIZZLE_API SwInitialize(const SwChar* appName = nullptr);
    SwBool SWIZZLE_API SwCleanup();

    /*void SWIZZLE_API SwInitialize(eRendererBackend graphicsBackend);
    void SWIZZLE_API SwCleanup();

    void SWIZZLE_API SwAddLogger(LoggerIfc* logger);

    uint32_t SWIZZLE_API SwGetNumDisplays();
    void SWIZZLE_API SwGetDisplayResolution(uint32_t displayIndex, uint32_t& resolutionCount, Resolution* resolutions);

    Resource<Window> SWIZZLE_API SwCreateWindow(uint32_t width, uint32_t height, const char* title);
    Resource<Renderer> SWIZZLE_API SwCreateRenderer(Window* window, eRendererBackend backend);*/

} // namespace swizzle

namespace sw = swizzle;

#endif