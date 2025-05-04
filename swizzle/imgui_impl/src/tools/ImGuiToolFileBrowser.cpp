
/* Include files */

#include <ImGuiSwizzleTools.hpp>

#include <codecvt>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#include <Windows.h>
#endif

/* Defines */

#define IS_BIT_SET(var, pos) ((var) & (1 << (pos)))

#ifdef _WIN32
#define HAS_LOGICAL_DRIVES true
#else
#define HAS_LOGICAL_DRIVES false
#endif

/* Typedefs */

/* Structs/Classes */

namespace imext
{
    struct FileBrowserCache
    {
        int mSelectedIndex = -1;
        bool mCacheInvalid = true;
        std::string mFileName;
        std::string mWorkingPath;
        std::string mLastPath;
        std::string mOrigPath;
        std::vector<DirectoryItem> mItems;
    };

    struct FileBrowserCacheStorage
    {
        std::unordered_map<const char*, FileBrowserCache> mCache;
    };

    class DefaultFileSystemInfo : public FileSystemInfo
    {
    public:
        virtual ~DefaultFileSystemInfo() {}

        virtual bool hasLogicalDrives() override
        {
            return HAS_LOGICAL_DRIVES;
        }

        virtual std::vector<std::string> getLogicalDrives() override
        {
            std::vector<std::string> driveLetters;

#ifdef _WIN32
            char* drives = nullptr;
            DWORD byteCount = GetLogicalDriveStringsA(0, nullptr);
            drives = (char*)malloc(byteCount);
            DWORD newCount = GetLogicalDriveStringsA(byteCount, drives);

            char* off = drives;
            DWORD c = 0;
            for (unsigned int i = 0; i < 25 && (c < newCount); ++i)
            {
                auto size = strlen(off) + 1;

                char dl[4]{};
                dl[0] = off[0];
                dl[1] = off[1];
                dl[2] = off[2];
                dl[3] = 0;

                driveLetters.push_back(dl);

                off += size;
                c += (DWORD)size;
            }

            free(drives);
#endif

            return driveLetters;
        }

        virtual std::filesystem::path absolutePath(const std::filesystem::path& path) override
        {
            return std::filesystem::absolute(path);
        }

        virtual std::filesystem::path getRootPath(const std::filesystem::path& path) override
        {
            return std::filesystem::absolute(path).root_path();
        }

        virtual std::filesystem::path getWihtoutRootPath(const std::filesystem::path& path) override
        {
            return std::filesystem::absolute(path).relative_path();
        }

        virtual std::filesystem::path getDirectory(const std::filesystem::path& path) override
        {
            std::filesystem::path newPath = path;
            auto dir = std::filesystem::directory_entry(path);
            if (dir.is_regular_file())
            {
                newPath = dir.path().parent_path();
            }
            return newPath;
        }

        virtual bool visible(const std::filesystem::path& path) override
        {
            bool visible = true;
#if _WIN32
            auto p = std::filesystem::u8path(path.c_str());

            DWORD attribs = GetFileAttributesW(p.wstring().c_str());
            if ((attribs & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN)
            {
                visible = false;
            }
#else
            if (path.u8string()[0] == '.')
            {
                visible = false;
            }
#endif
            return visible;
        }

        virtual bool exists(const std::filesystem::path& path) override
        {
            return std::filesystem::directory_entry(path).exists();
        }

        virtual bool isRoot(const std::filesystem::path& path) override
        {
            auto rootPath = std::filesystem::directory_entry(path).path().root_path().u8string();
            return rootPath == path;
        }

        virtual bool isDirectory(const std::filesystem::path& path) override
        {
            return std::filesystem::directory_entry(path).is_directory();
        }

        virtual std::vector<DirectoryItem> getDirectoryItems(const std::filesystem::path& path, bool onlyDirectories) override
        {
            std::vector<DirectoryItem> items;
            std::filesystem::directory_iterator dirIter(path);

            for (auto& d : dirIter)
            {
                if (!visible(d.path()))
                {
                    continue;
                }
                auto name = d.path().filename();
                DirectoryType type{};
                if (d.is_directory())
                {
                    type = DirectoryType::Directory;
                }
                else if (d.is_regular_file())
                {
                    type = DirectoryType::File;
                }
                else
                {
                    type = DirectoryType::Unknown;
                }

                if (onlyDirectories && type == DirectoryType::Directory)
                {
                    items.push_back({name, type});
                }
                else if (!onlyDirectories)
                {
                    items.push_back({name, type});
                }
            }

            std::sort(items.begin(), items.end(),
                      [](const auto& l, const auto& r) { return U32(l.mType) > U32(r.mType); });

            return items;
        }

        virtual void createDir(const std::filesystem::path& path) override
        {
            std::filesystem::create_directories(path);
        }

        virtual std::string traverseTo(const std::filesystem::path& path, const std::string& dir) override
        {
            //auto newPath = std::filesystem::u8path(path / dir);
            return path/dir;
        }
    };

} // namespace imext

/* Static Variables */

namespace imext
{
    FileBrowserCacheStorage gFileBrowserCache;
}

/* Static Function Declaration */

namespace imext
{
    static std::string GetDirectoryTypeAsString(DirectoryType type);

    static void DrawDriveComboBox(FileSystemInfo* fsInfo, std::filesystem::path& path);
    static void DrawFilePath(FileSystemInfo* fsInfo, std::filesystem::path& path, FileBrowserCache& cache);
    static FileBrowserStatus DrawFileTable(FileSystemInfo* fsInfo, std::filesystem::path& path, FileBrowserCache& cache);
} // namespace imext

/* Static Function Definition */

namespace imext
{
    static std::string GetDirectoryTypeAsString(DirectoryType type)
    {
        std::string typeName = "";
        switch (type)
        {
        case imext::DirectoryType::File:
            typeName = "File";
            break;
        case imext::DirectoryType::Directory:
            typeName = "Directory";
            break;
        case imext::DirectoryType::Unknown:
        default:
            typeName = "Unknown";
            break;
        }

        return typeName;
    }

    static void DrawDriveComboBox(FileSystemInfo* fsInfo, std::filesystem::path& path)
    {
        auto rootName = fsInfo->getRootPath(path);
        auto logicalDrives = fsInfo->getLogicalDrives();
        //auto textSize = ImGui::CalcTextSize(rootName.c_str());
        ImGui::SetNextItemWidth(45.0f);
        if (ImGui::BeginCombo("##Drive", rootName.c_str()))
        {
            for (size_t i = 0ull; i < logicalDrives.size(); ++i)
            {
                const auto& driveName = logicalDrives[i];
                ImGui::PushID((int)i);
                if (ImGui::Selectable(driveName.c_str(), rootName == driveName))
                {
                    path = driveName + "/";
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
    }

    static void DrawFilePath(FileSystemInfo* fsInfo, std::filesystem::path& path, FileBrowserCache& cache)
    {
        auto rootPath = fsInfo->getRootPath(path);
        auto withoutRoot = fsInfo->getWihtoutRootPath(path);
        if (cache.mWorkingPath == "")
        {
            cache.mWorkingPath = withoutRoot;
        }

        auto size = ImGui::GetWindowSize();
        ImGui::Text("%s", rootPath.c_str());
        auto pos = ImGui::GetCursorPos();
        ImGui::SameLine();
        ImGui::SetNextItemWidth(size.x - pos.x);
        if (InputText("##Path", cache.mWorkingPath))
        {
            if (fsInfo->exists(rootPath / cache.mWorkingPath))
            {
                withoutRoot = cache.mWorkingPath;
            }
            else
            {
                ImGui::OpenPopup("Create Directory");
            }
        }
        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowMinSize, ImVec2{200.0f, 100.0f});
        ImGui::SetNextWindowSize({200.0f, 100.0f});
        ImGui::SetNextWindowSizeConstraints({200.0f, 50.0f}, {200.0f, 100.0f});
        auto cd = ConfirmModalPopup("Create Directory", ImGuiWindowFlags_::ImGuiWindowFlags_NoMove, "Create directory?",
                                    {"Yes", "No"});
        if (cd == 0)
        {
            fsInfo->createDir(rootPath / cache.mWorkingPath);
            withoutRoot = cache.mWorkingPath;
        }
        else if (cd == 1)
        {
            cache.mWorkingPath = withoutRoot;
        }

        ImGui::PopStyleVar();
        path = rootPath / withoutRoot;
    }

    static FileBrowserStatus DrawFileTable(FileSystemInfo* fsInfo, std::filesystem::path& path, FileBrowserCache& cache)
    {
        FileBrowserStatus returnCode = FileBrowserStatus::None;
        auto size = ImGui::GetWindowSize();
        auto pos = ImGui::GetCursorPos();

        if (ImGui::BeginTable("##fileList", 3,
                              ImGuiTableFlags_::ImGuiTableFlags_ScrollY | ImGuiTableFlags_::ImGuiTableFlags_Resizable,
                              {size.x - (pos.x * 2.0f), size.y - (pos.y * 2.0f)}))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            bool clearFileName = false;

            int counter = 0;
            if (!fsInfo->isRoot(path))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(counter++);
                if (ImGui::Selectable("..", cache.mSelectedIndex == counter,
                                      ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns |
                                          ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick) &&
                    ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
                {
                    path = fsInfo->traverseTo(path, "..");
                    cache.mSelectedIndex = -1;
                    cache.mWorkingPath = "";
                    clearFileName = true;
                }
                if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
                {
                    cache.mSelectedIndex = counter;
                }
                ImGui::PopID();
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", GetDirectoryTypeAsString(DirectoryType::Directory).c_str());
            }

            for (const auto& itm : cache.mItems)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(counter++);
                if (ImGui::Selectable(itm.mPath.c_str(), cache.mSelectedIndex == counter,
                                      ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns |
                                          ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick) &&
                    ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
                {
                    if (itm.mType == DirectoryType::Directory)
                    {
                        path = fsInfo->traverseTo(path, itm.mPath);
                        cache.mSelectedIndex = -1;
                        cache.mWorkingPath = "";
                        clearFileName = true;
                    }
                    else if (itm.mType == DirectoryType::File)
                    {
                        cache.mFileName = itm.mPath;
                        returnCode = FileBrowserStatus::Selected;
                    }
                }
                if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
                {
                    cache.mSelectedIndex = counter;
                    cache.mFileName = itm.mPath;
                }
                ImGui::PopID();
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", GetDirectoryTypeAsString(itm.mType).c_str());
            }

            if (clearFileName)
            {
                cache.mFileName = "";
                cache.mSelectedIndex = -1;
            }

            ImGui::EndTable();
        }
        return returnCode;
    }

} // namespace imext

/* Function Definition */

namespace imext
{
    IMGUI_IMPL_API FileBrowserStatus FileBrowser(const char* name, bool* open, std::filesystem::path& path, FileBrowserMode mode,
                                                 FileSystemInfo* fsInfo)
    {
        DefaultFileSystemInfo defaultFsInfo{};
        if (!fsInfo)
        {
            fsInfo = &defaultFsInfo;
        }

        FileBrowserCache& cache = gFileBrowserCache.mCache[name];

        if (cache.mOrigPath == "")
        {
            cache.mOrigPath = path;
        }

        path = fsInfo->absolutePath(path);
        path = fsInfo->getDirectory(path);

        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowMinSize, ImVec2{600, 400});
        ImGui::Begin(name, open);

        if (fsInfo->hasLogicalDrives())
        {
            DrawDriveComboBox(fsInfo, path);
        }

        // Update cache
        if (fsInfo->exists(path) && (cache.mLastPath != path || cache.mCacheInvalid))
        {
            cache.mItems.clear();
            bool dirOnly = (mode == FileBrowserMode::OpenDirectory) || (mode == FileBrowserMode::SaveDirectory);

            cache.mItems = fsInfo->getDirectoryItems(path, dirOnly);

            cache.mLastPath = path;
            cache.mCacheInvalid = false;
        }

        // Render table from cache
        DrawFilePath(fsInfo, path, cache);
        FileBrowserStatus returnCode = DrawFileTable(fsInfo, path, cache);
        auto size = ImGui::GetWindowSize();
        auto pos = ImGui::GetCursorPos();
        ImGui::SetNextItemWidth(size.x - (pos.x * 2.0f));
        InputText("##SelectedFile", cache.mFileName);

        std::string buttonText = "Save";

        if (mode == FileBrowserMode::OpenFile || mode == FileBrowserMode::OpenDirectory)
        {
            buttonText = "Open";
        }

        if (ImGui::Button(buttonText.c_str()))
        {
            returnCode = FileBrowserStatus::Selected;
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            returnCode = FileBrowserStatus::Cancel;
        }

        ImGui::End();
        ImGui::PopStyleVar();

        if (returnCode == FileBrowserStatus::Selected)
        {
            auto tmpFilePath = fsInfo->traverseTo(path, cache.mFileName);
            if (fsInfo->isDirectory(tmpFilePath) && (mode != FileBrowserMode::OpenDirectory) &&
                (mode != FileBrowserMode::SaveDirectory))
            {
                returnCode = FileBrowserStatus::None;
                cache.mFileName = "";
                cache.mSelectedIndex = -1;
            }

            if (!fsInfo->exists(tmpFilePath) &&
                ((mode == FileBrowserMode::OpenFile) || (mode == FileBrowserMode::OpenDirectory)))
            {
                returnCode = FileBrowserStatus::None;
                ImGui::OpenPopup("Not Found");
            }

            // Save confirm overwrite
            if ((mode == FileBrowserMode::SaveFile) && fsInfo->exists(tmpFilePath))
            {
                returnCode = FileBrowserStatus::None;
                ImGui::OpenPopup("Confirm Overwrite");
            }
        }

        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowMinSize, ImVec2{200.0f, 100.0f});
        ImGui::SetNextWindowSize({200.0f, 100.0f});
        ImGui::SetNextWindowSizeConstraints({200.0f, 50.0f}, {200.0f, 100.0f});
        ConfirmModalPopup("Not Found", ImGuiWindowFlags_::ImGuiWindowFlags_NoMove,
                          "Ensure that the selected\nitem exists!", {"Ok"});
        ImGui::PopStyleVar();

        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowMinSize, ImVec2{200.0f, 100.0f});
        ImGui::SetNextWindowSize({200.0f, 100.0f});
        ImGui::SetNextWindowSizeConstraints({200.0f, 50.0f}, {200.0f, 100.0f});
        auto r = ConfirmModalPopup("Confirm Overwrite", ImGuiWindowFlags_::ImGuiWindowFlags_NoMove,
                                   "Overwrite Selected File?", {"Yes", "No"});
        if (r == 0)
        {
            returnCode = FileBrowserStatus::Selected;
        }

        ImGui::PopStyleVar();

        if (returnCode != FileBrowserStatus::None)
        {
            if (open)
            {
                *open = false;
            }
        }
        if (returnCode == FileBrowserStatus::Cancel)
        {
            path = cache.mOrigPath;
            cache.mOrigPath = "";
            cache.mWorkingPath = "";
        }
        if (returnCode == FileBrowserStatus::Selected)
        {
            path = fsInfo->traverseTo(path, cache.mFileName);
            cache.mOrigPath = "";
            cache.mWorkingPath = "";
        }
        if (open)
        {
            if ((*open == false) && (returnCode == FileBrowserStatus::None))
            {
                path = cache.mOrigPath;
                cache.mOrigPath = "";
                cache.mWorkingPath = "";
            }
        }

        return returnCode;
    }

} // namespace imext

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
