
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
        std::string mPath;
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

        virtual std::string absolutePath(const std::string& path) = 0;
        virtual std::string getRootPath(const std::string& path) = 0;
        virtual std::string getWihtoutRootPath(const std::string& path) = 0;
        virtual std::string getDirectory(const std::string& path) = 0;
        virtual bool visible(const std::string& path) = 0;
        virtual bool exists(const std::string& path) = 0;
        virtual bool isRoot(const std::string& path) = 0;
        virtual bool isDirectory(const std::string& path) = 0;
        virtual std::vector<DirectoryItem> getDirectoryItems(const std::string& path, bool onlyDirectories) = 0;

        virtual void createDir(const std::string& path) = 0;
        virtual std::string traverseTo(const std::string& path, const std::string& dir) = 0;
    };
} // namespace imext

/* Function Declaration */

namespace imext
{
    IMGUI_IMPL_API FileBrowserStatus FileBrowser(const char* name, bool* open, std::string& path,
                                                 FileBrowserMode mode = FileBrowserMode::OpenFile,
                                                 FileSystemInfo* fsInfo = nullptr);
    IMGUI_IMPL_API bool InputText(const char* label, std::string& str);
    IMGUI_IMPL_API int ConfirmModalPopup(const char* label, ImGuiWindowFlags_ flags, const std::string& description,
                                         const std::vector<std::string>& options);
} // namespace imext