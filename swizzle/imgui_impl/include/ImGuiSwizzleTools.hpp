
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

    enum class InputType
    {
        Float,
        Vec2,
        Vec3,
        Vec4,
        Color
    };

} // namespace imext

/* Forward Declared Structs/Classes */

namespace imext
{
    class ShaderNode;
}

/* Struct Declaration */

namespace imext
{
    struct DirectoryItem
    {
        std::string mPath;
        DirectoryType mType;
    };

    struct Input
    {
        std::string mText;
        InputType mInputType;
        float mInputData[4];
        bool mPluggable;
        ImVec2 mPos;
        ImVec2 mSize;
        std::weak_ptr<ShaderNode> mSource;
        U32 mSourceIndex;
    };

    struct Output
    {
        std::string mText;
        ImVec2 mPos;
        ImVec2 mSize;
    };

    struct ShaderLink
    {
        ImVec2 mStartPos;
        ImVec2 mEndPos;
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

    class ShaderNode
    {
    public:
        virtual ~ShaderNode() {}

        virtual const ImVec2& getPos() = 0;
        virtual void setPos(const ImVec2& pos) = 0;

        virtual const ImVec2& getSize() = 0;
        virtual void setSize(ImVec2& size) = 0;

        virtual std::vector<Input>& getInputs() = 0;
        virtual std::vector<Output>& getOutputs() = 0;
    };

    class NodeCollection
    {
    public:
        virtual ~NodeCollection() {}

        virtual const std::string& getName() const = 0;
    };

    class ShaderGraphController
    {
    public:
        virtual ~ShaderGraphController() {}

        virtual std::vector<std::shared_ptr<NodeCollection>>& getNodeCollection() = 0;

        virtual std::vector<std::shared_ptr<ShaderNode>>& getNodes() = 0;
    };

} // namespace imext

/* Function Declaration */

namespace imext
{
    IMGUI_IMPL_API FileBrowserStatus FileBrowser(const char* name, bool* open, std::string& path,
                                                 FileBrowserMode mode = FileBrowserMode::OpenFile,
                                                 FileSystemInfo* fsInfo = nullptr);
    IMGUI_IMPL_API bool InputText(const char* label, std::string& str);
    IMGUI_IMPL_API bool ColorPickerbutton4(const char* label, float* col, ImGuiColorEditFlags flags = 0);
    IMGUI_IMPL_API int ConfirmModalPopup(const char* label, ImGuiWindowFlags_ flags, const std::string& description,
                                         const std::vector<std::string>& options);

    IMGUI_IMPL_API void ShaderGraph(const char* name, bool* open, ShaderGraphController* sgc);
} // namespace imext