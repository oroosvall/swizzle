#ifdef SW_LINUX

#include <swizzle/core/Logging.hpp>
#include "PlatformLayer.hpp"

#if defined(SW_LINUX_XLIB)
#include "Linux/x/X11Window.hpp"
#elif defined(SW_LINUX_XCB)
#include "Linux/xcb/XcbWindow.hpp"
#endif

namespace swizzle::core
{
    namespace platform
    {
        void platformInit(const SwChar* appName)
        {
            UNUSED_ARG(appName);
            LOG_INFO("platformInit() Linux");
        }

        void platformDeInit()
        {

        }

        const SwChar* getPlatformSaveGameDirectory()
        {
            return "";
        }

        const SwChar* getPlatformTempDirectory()
        {
            return "";
        }

        const SwChar* getPlatformAppCacheDirectory()
        {
            return "";
        }

        U32 getPlatformDisplayCount()
        {
            return 0u;
        }

        U32 getPlatformDisplayResolutionCount(U32 displayIndex)
        {
            UNUSED_ARG(displayIndex);
            return 0u;
        }

        void getPlatformDisplayResolutions(U32 displayIndex, Resolution* resolutions, U32 size)
        {
            UNUSED_ARG(displayIndex);
            UNUSED_ARG(resolutions);
            UNUSED_ARG(size);
        }

        common::Resource<SwWindow> createPlatformWindow(const U32 width, const U32 height, const SwChar* title)
        {
            UNUSED_ARG(width);
            UNUSED_ARG(height);
            UNUSED_ARG(title);
            #if defined(SW_LINUX_XLIB)
            return std::make_shared<XlibWindow>(width, height, title);
            #elif defined(SW_LINUX_XCB)
            return std::make_shared<XcbWindow>(width, height, title);
            #endif
        }

        U64 getPlatformTimeStampMs()
        {
            return 0u;
        }

        U64 getPlatformTimeStampUs()
        {
            return 0u;
        }

        U32 getPlatformCurrentThreadId()
        {
            return 0u;
        }

        const SwChar* GetPlatformKeyText(S32 scanCode)
        {
            UNUSED_ARG(scanCode);
            return "";
        }

        const SwWChar* GetPlatformKeyTextW(S32 scanCode)
        {
            UNUSED_ARG(scanCode);
            return L"";
        }

        S32 PlatformKeyToScanCode(input::Keys key)
        {
            UNUSED_ARG(key);
            return 0;
        }

        void PlatformShowCriticalMessage(const SwChar* text)
        {
            UNUSED_ARG(text);
        }

        void PlatformShowCriticalMessageW(const SwWChar* text)
        {
            UNUSED_ARG(text);
        }

    } // namespace platform

} // namespace swizzle

#endif