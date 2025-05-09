#ifdef SW_WINDOWS

#include <swizzle/core/Logging.hpp>
#include "PlatformLayer.hpp"

#include "Windows/Win32Window.hpp"
#include "Windows/Win32Input.hpp"
#include "Windows/Win32File.hpp"

#include <cstdlib>

#include <UserEnv.h>
#pragma comment(lib, "Userenv.lib")

#include <ShlObj.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include "utils/StringUtils.hpp"
#include <processthreadsapi.h>
#include <KnownFolders.h>

namespace swizzle::core
{
    namespace platform
    {
        constexpr U64 NULL_AND_SLASH_SIZE = 3u;

        LARGE_INTEGER gFrequency;
        SwChar* gSaveGamesDir = nullptr;
        SwChar* gTempDir = nullptr;
        SwChar* gAppDataDir = nullptr;

        void platformInit(const SwChar* appName)
        {
            appName;
            LOG_INFO("platformInit() Windows");
            WNDCLASS cls;
            (void)memset(&cls, 0, sizeof(cls));

            cls.style = CS_HREDRAW | CS_VREDRAW;
            cls.lpfnWndProc = win32::WindowProc;
            cls.cbClsExtra = 0;
            cls.cbWndExtra = 0;
            cls.hInstance = GetModuleHandle(NULL);
            cls.hIcon = NULL;
            cls.hCursor = LoadCursor(NULL, IDC_ARROW);
            cls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
            cls.lpszMenuName = NULL;
            cls.lpszClassName = win32::gWindowClass;

            QueryPerformanceFrequency(&gFrequency);

            if (RegisterClass(&cls) == 0)
            {
                LOG_ERROR("Failed to register window class");
            }

            U64 appNameLen = utils::strlenSafe(appName);

            PWSTR saveGames = NULL;

            if (SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_DEFAULT, GetCurrentProcessToken(), &saveGames) != S_OK)
            {
                LOG_ERROR("Failed to get home path");
            }
            else
            {
                U64 dirLen = utils::wstrlenSafe(saveGames);
                U64 newSize = dirLen + appNameLen + NULL_AND_SLASH_SIZE;
                gSaveGamesDir = new SwChar[newSize];
                U64 out = 0;
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
                U64 out = 0;
                U64 dirLen = utils::wstrlenSafe(appData);
                U64 newSize = dirLen + appNameLen + NULL_AND_SLASH_SIZE;
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

            CreateDirectoryA(gTempDir, NULL);
            CreateDirectoryA(gSaveGamesDir, NULL);
            CreateDirectoryA(gAppDataDir, NULL);

            InputInit();
        }

        void platformDeInit()
        {
            InputCleanup();
            delete []gSaveGamesDir;
            delete []gTempDir;
            delete []gAppDataDir;

            gSaveGamesDir = nullptr;
            gTempDir = nullptr;
            gAppDataDir = nullptr;

            if (UnregisterClass(win32::gWindowClass, GetModuleHandle(NULL)) == 0)
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
            U32* count = (U32*)dwData;
            (*count)++;
            return TRUE;
        }

        U32 getMonitorCount()
        {
            U32 count = 0U;
            EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&count);
            return count;
        }

        MonitorInfo getMonitorInfo(U32 index)
        {
            UNUSED_ARG(index);

            DEVMODE devmode = {};
            devmode.dmSize = sizeof(DEVMODE);

            EnumDisplaySettingsExW(NULL, ENUM_CURRENT_SETTINGS, &devmode, EDS_RAWMODE);

            MonitorInfo res{};
            res.mHeight = devmode.dmPelsHeight;
            res.mWidth = devmode.dmPelsWidth;
            res.mXPos = devmode.dmPosition.x;
            res.mYPos = devmode.dmPosition.y;

            return res;
        }

        U32 getPlatformDisplayCount()
        {
            U32 count = 0U;

            EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&count);

            return count;
        }

        U32 getPlatformDisplayResolutionCount(U32 displayIndex)
        {
            displayIndex;
            U32 count = 0U;

            DEVMODE deviceMode = { 0U };
            deviceMode.dmSize = sizeof(DEVMODE);
            deviceMode.dmDriverExtra = 0;

            while (EnumDisplaySettings(NULL, count, &deviceMode) != 0)
            {
                count++;
            }
            return count;
        }

        void getPlatformDisplayResolutions(U32 displayIndex, Resolution* resolutions, U32 size)
        {
            displayIndex;
            U32 count = 0U;

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

        common::Resource<SwWindow> createPlatformWindow(const U32 width, const U32 height, const SwChar* title)
        {
            return std::make_shared<win32::Win32Window>(width, height, title);
        }

        U64 getPlatformTimeStampMs()
        {
            LARGE_INTEGER time;
            QueryPerformanceCounter(&time);

            time.QuadPart *= 1000U;
            time.QuadPart /= gFrequency.QuadPart;

            return static_cast<uint64_t>(time.QuadPart);
        }

        U64 getPlatformTimeStampUs()
        {
            LARGE_INTEGER time;
            QueryPerformanceCounter(&time);

            time.QuadPart *= 1000000U;
            time.QuadPart /= gFrequency.QuadPart;

            return static_cast<uint64_t>(time.QuadPart);
        }

        U32 getPlatformCurrentThreadId()
        {
            const DWORD threadId = ::GetCurrentThreadId();
            return static_cast<const U32>(threadId);
        }

        const SwChar* GetPlatformKeyText(S32 scanCode)
        {
            return GetWinKeyText(scanCode);
        }

        const SwWChar* GetPlatformKeyTextW(S32 scanCode)
        {
            return GetWinKeyTextW(scanCode);
        }

        S32 PlatformKeyToScanCode(input::Keys key)
        {
            return Key2ScanCode(key);
        }

        void PlatformShowCriticalMessage(const SwChar* text)
        {
            MessageBoxA(NULL, text, "Error", MB_ICONERROR | MB_OK);
            terminate();
        }

        void PlatformShowCriticalMessageW(const SwWChar* text)
        {
            MessageBoxW(NULL, text, L"Error", MB_ICONERROR | MB_OK);
            terminate();
        }

        common::Resource<IFile> OpenFile(const SwChar* path, FileMode mode)
        {
            return common::CreateRef<Win32File>(path, mode);
        }

        common::Resource<IFile> OpenFileW(const SwWChar* path, FileMode mode)
        {
            return common::CreateRef<Win32File>(path, mode);
        }


    } // namespace platform

} // namespace swizzle

#endif