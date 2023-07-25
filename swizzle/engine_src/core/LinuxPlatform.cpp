#ifdef SW_LINUX

#include <swizzle/core/Logging.hpp>
#include "PlatformLayer.hpp"

#include "Linux/LinuxFile.hpp"

#if defined(SW_LINUX_XLIB)
#include "Linux/x/X11Window.hpp"
#include "Linux/x/X11Input.hpp"
#elif defined(SW_LINUX_XCB)
#include "Linux/xcb/XcbWindow.hpp"

namespace swizzle::core
{
    void InputInit() {}
    void InputCleanup() {}

    input::Keys ScanCode2Key(S32 scanCode){UNUSED_ARG(scanCode)}

    S32 Key2ScanCode(input::Keys key){UNUSED_ARG(key)}

    const SwChar* GetWinKeyText(S32 scanCode){UNUSED_ARG(scanCode);return "";}
    const SwWChar* GetWinKeyTextW(S32 scanCode){UNUSED_ARG(scanCode);return L"";}
} // namespace swizzle::core

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

        U32 getMonitorCount()
        {
            return 0u;
        }

        MonitorInfo getMonitorInfo(U32 index)
        {
            UNUSED_ARG(index);
            MonitorInfo res{};
            return res;
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
            return Key2ScanCode(key);
        }

        void PlatformShowCriticalMessage(const SwChar* text)
        {
            UNUSED_ARG(text);
        }

        void PlatformShowCriticalMessageW(const SwWChar* text)
        {
            UNUSED_ARG(text);
        }

        common::Resource<IFile> OpenFile(const SwChar* path, FileMode mode)
        {
            return common::CreateRef<LinuxFile>(path, mode);
        }

        common::Resource<IFile> OpenFileW(const SwWChar* path, FileMode mode)
        {
            return common::CreateRef<LinuxFile>(path, mode);
        }

    } // namespace platform

} // namespace swizzle

#endif