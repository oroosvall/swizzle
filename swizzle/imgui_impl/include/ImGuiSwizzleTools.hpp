
#pragma once

/* Include files */

#include <imgui/imgui.h>
#include <swizzle/gfx/Context.hpp>

#include <filesystem>
#include <string>

/* Defines */

/* Typedefs/enums */

namespace imext
{
    enum class DirectoryType
    {
        File,
        Directory,
        Unknown
    };

    enum class FileBrowserMode
    {
        OpenFile,
        OpenDirectory,
        SaveFile,
        SaveDirectory
    };

    enum class FileBrowserStatus
    {
        None,
        Selected,
        Cancel
    };

} // namespace imext

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace imext
{

    struct DirectoryItem
    {
        std::filesystem::path mPath;
        DirectoryType mType;
    };

} // namespace imext

/* Class Declaration */

namespace imext
{
    class FileSystemInfo
    {
    public:
        virtual ~FileSystemInfo() {}

        virtual bool hasLogicalDrives() = 0;
        virtual std::vector<std::string> getLogicalDrives() = 0;

        virtual std::filesystem::path absolutePath(const std::filesystem::path& path) = 0;
        virtual std::filesystem::path getRootPath(const std::filesystem::path& path) = 0;
        virtual std::filesystem::path getWihtoutRootPath(const std::filesystem::path& path) = 0;
        virtual std::filesystem::path getDirectory(const std::filesystem::path& path) = 0;
        virtual bool visible(const std::filesystem::path& path) = 0;
        virtual bool exists(const std::filesystem::path& path) = 0;
        virtual bool isRoot(const std::filesystem::path& path) = 0;
        virtual bool isDirectory(const std::filesystem::path& path) = 0;
        virtual std::vector<DirectoryItem> getDirectoryItems(const std::filesystem::path& path, bool onlyDirectories) = 0;

        virtual void createDir(const std::filesystem::path& path) = 0;
        virtual std::string traverseTo(const std::filesystem::path& path, const std::string& dir) = 0;
    };
} // namespace imext

/* Function Declaration */

namespace imext
{
    IMGUI_IMPL_API FileBrowserStatus FileBrowser(const char* name, bool* open, std::filesystem::path& path,
                                                 FileBrowserMode mode = FileBrowserMode::OpenFile,
                                                 FileSystemInfo* fsInfo = nullptr);
    IMGUI_IMPL_API bool InputText(const char* label, std::string& str);
    IMGUI_IMPL_API int ConfirmModalPopup(const char* label, ImGuiWindowFlags_ flags, const std::string& description,
                                         const std::vector<std::string>& options);
} // namespace imext