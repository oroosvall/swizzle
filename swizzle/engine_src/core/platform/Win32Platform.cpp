#ifdef SW_WINDOWS

#include <swizzle/core/logging/Logging.hpp>
#include "PlatformLayer.hpp"
//#include <swizzle/Platform.hpp>

#include "Windows/Win32Window.hpp"

#include <UserEnv.h>
#pragma comment(lib, "Userenv.lib")

#include <ShlObj.h>
#include "Shlwapi.h"

#include <cstdlib>
#pragma comment(lib, "Shlwapi.lib")

//#include "gfxOpengl/GlRenderer.hpp"
//#include "gfxVulkan/VulkanRenderer.hpp"
//
//#include "../Logging/Logger.hpp"

#include "utils/StringUtils.hpp"

namespace swizzle::core
{

    namespace platform
    {

        constexpr SwU64 NULL_AND_SLASH_SIZE = 3;

        LARGE_INTEGER gFrequency;
        SwChar* gSaveGamesDir = nullptr;
        SwChar* gTempDir = nullptr;
        SwChar* gAppDataDir = nullptr;

        void platformInit(const SwChar* appName)
        {
            appName;
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

            SwU64 appNameLen = utils::strlenSafe(appName);

            PWSTR saveGames = NULL;
            if (SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_DEFAULT, GetCurrentProcessToken(), &saveGames) != S_OK)
            {
                LOG_ERROR("Failed to get home path");
            }
            else
            {
                SwU64 dirLen = utils::wstrlenSafe(saveGames);
                SwU64 newSize = dirLen + appNameLen + NULL_AND_SLASH_SIZE;
                gSaveGamesDir = new SwChar[newSize];
                SwU64 out = 0;
                wcstombs_s(&out, gSaveGamesDir, newSize, saveGames, dirLen);
                strncat_s(gSaveGamesDir, newSize, "\\", 1u);
                strncat_s(gSaveGamesDir, newSize, appName, appNameLen);
                strncat_s(gSaveGamesDir, newSize, "\\", 1u);
            }
            CoTaskMemFree(saveGames);

            PWSTR appData = NULL;
            if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, GetCurrentProcessToken(), &appData) != S_OK)
            {
                LOG_ERROR("Failed to get appdata path");
            }
            else
            {
                SwU64 out = 0;
                SwU64 dirLen = utils::wstrlenSafe(appData);
                SwU64 newSize = dirLen + appNameLen + NULL_AND_SLASH_SIZE;
                gAppDataDir = new SwChar[newSize];
                wcstombs_s(&out, gAppDataDir, newSize, appData, dirLen);
                strncat_s(gAppDataDir, newSize, "\\", 1u);
                strncat_s(gAppDataDir, newSize, appName, appNameLen);
                strncat_s(gAppDataDir, newSize, "\\", 1u);
            }
            CoTaskMemFree(appData);

            gTempDir = new SwChar[MAX_PATH];
            if (GetTempPathA(MAX_PATH, gTempDir) == 0)
            {
                LOG_ERROR("Failed to get temp path");
            }
            else
            {
                strncat_s(gTempDir, MAX_PATH, "\\", 1u);
                strncat_s(gTempDir, MAX_PATH, appName, appNameLen);
                strncat_s(gTempDir, MAX_PATH, "\\", 1u);
            }
        }

        void platformDeInit()
        {
            delete []gSaveGamesDir;
            delete []gTempDir;
            delete []gAppDataDir;

            gSaveGamesDir = nullptr;
            gTempDir = nullptr;
            gAppDataDir = nullptr;

            if (UnregisterClass(gWindowClass, GetModuleHandle(NULL)) == 0)
            {
                LOG_ERROR("Failed to free window class");
            }
        }

        const SwChar* getPlatformSaveGameDirectory()
        {
            return gSaveGamesDir;
        }

        const SwChar* getPlatformTempDirectory()
        {
            return gTempDir;
        }

        const SwChar* getPlatformAppCacheDirectory()
        {
            return gAppDataDir;
        }

        BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
        {
            hMonitor;
            hdcMonitor;
            lprcMonitor;
            SwU32* count = (SwU32*)dwData;
            (*count)++;
            return TRUE;
        }

        SwU32 getPlatformDisplayCount()
        {
            SwU32 count = 0U;

            EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&count);

            return count;
        }

        SwU32 getPlatformDisplayResolutionCount(SwU32 displayIndex)
        {
            displayIndex;
            SwU32 count = 0U;

            DEVMODE deviceMode = { 0U };
            deviceMode.dmSize = sizeof(DEVMODE);
            deviceMode.dmDriverExtra = 0;

            while (EnumDisplaySettings(NULL, count, &deviceMode) != 0)
            {
                count++;
            }
            return count;
        }

        void getPlatformDisplayResolutions(SwU32 displayIndex, Resolution* resolutions, SwU32 size)
        {
            displayIndex;
            SwU32 count = 0U;

            DEVMODE deviceMode = { 0U };
            deviceMode.dmSize = sizeof(DEVMODE);
            deviceMode.dmDriverExtra = 0;

            if (resolutions != nullptr)
            {
                while (EnumDisplaySettings(NULL, count, &deviceMode) != 0 && count != size)
                {
                    resolutions[count].mHeight = deviceMode.dmPelsHeight;
                    resolutions[count].mWidth = deviceMode.dmPelsWidth;
                    resolutions[count].mFrequency = deviceMode.dmDisplayFrequency;
                    count++;
                }
            }
        }

        Resource<Window> createPlatformWindow(const SwU32 width, const SwU32 height, const SwChar* title)
        {
            return std::make_shared<Win32Window>(width, height, title);
        }
    
        const SwU64 getPlatformTimeStampMs()
        {
            LARGE_INTEGER time;
            QueryPerformanceCounter(&time);

            time.QuadPart *= 1000000U;
            time.QuadPart /= gFrequency.QuadPart;

            return static_cast<uint64_t>(time.QuadPart);
        }

        const SwU32 getPlatformCurrentThreadId()
        {
            const DWORD threadId = GetCurrentThreadId();
            return static_cast<const SwU32>(threadId);
        }
    } // namespace platform

} // namespace swizzle

#endif