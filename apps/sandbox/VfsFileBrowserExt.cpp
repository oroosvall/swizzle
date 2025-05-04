
/* Include files */

#include "VfsFileBrowserExt.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

VfsFileSystemInfo::VfsFileSystemInfo(common::Resource<swizzle::asset2::Vfs> vfs)
    : mVfs(vfs)
{
}

bool VfsFileSystemInfo::hasLogicalDrives()
{
    return false;
}

std::vector<std::string> VfsFileSystemInfo::getLogicalDrives()
{
    return std::vector<std::string>();
}

std::filesystem::path VfsFileSystemInfo::absolutePath(const std::filesystem::path& path)
{
    return path;
}

std::filesystem::path VfsFileSystemInfo::getRootPath(const std::filesystem::path& path)
{
    UNUSED_ARG(path);
    return std::filesystem::path("/");
}

std::filesystem::path VfsFileSystemInfo::getWihtoutRootPath(const std::filesystem::path& path)
{
    std::string np = path.c_str();
    if (path.c_str()[0] == '/')
    {
        np.erase(0, 1);
    }
    return std::filesystem::path(np.c_str());
}

std::filesystem::path VfsFileSystemInfo::getDirectory(const std::filesystem::path& path)
{
    if (mVfs->isDirectory(path.c_str()))
    {
        return path;
    }
    else
    {
        const SwChar* name = mVfs->getDirectory(path.c_str());
        if (name)
        {
            return mVfs->getDirectory(path.c_str());
        }
    }
    return std::filesystem::path("");
}

bool VfsFileSystemInfo::visible(const std::filesystem::path& path)
{
    UNUSED_ARG(path);
    return true;
}

bool VfsFileSystemInfo::exists(const std::filesystem::path& path)
{
    return mVfs->exists(path.c_str());
}

bool VfsFileSystemInfo::isRoot(const std::filesystem::path& path)
{
    return path.u8string() == u8"/";
}

bool VfsFileSystemInfo::isDirectory(const std::filesystem::path& path)
{
    return mVfs->isDirectory(path.c_str());
}

std::vector<imext::DirectoryItem> VfsFileSystemInfo::getDirectoryItems(const std::filesystem::path& path, bool onlyDirectories)
{
    std::vector<imext::DirectoryItem> items;
    U32 count = mVfs->getDirectoryItems(path.c_str());

    for (U32 i = 0; i < count; i++)
    {
        const SwChar* name = mVfs->getDirectoryItemName(path.c_str(), i);
        std::filesystem::path newPath = path / name;
        imext::DirectoryItem di{};
        di.mPath = name;
        di.mType = mVfs->isDirectory(newPath.c_str()) ? imext::DirectoryType::Directory : imext::DirectoryType::File;

        if (onlyDirectories)
        {
            if (di.mType == imext::DirectoryType::Directory)
            {
                items.push_back(di);
            }
        }
        else
        {
            items.push_back(di);
        }
    }

    return items;
}

void VfsFileSystemInfo::createDir(const std::filesystem::path& path)
{
    mVfs->createDirectory(path.c_str());
}

std::string VfsFileSystemInfo::traverseTo(const std::filesystem::path& path, const std::string& dir)
{
    std::string np = path.c_str();
    if (dir == "..")
    {
        if (np[np.size() - 1u] == '/')
        {
            np.pop_back();
        }
        const SwChar* name = mVfs->getDirectory(np.c_str());
        return name;
    }

    if (np[np.size() - 1u] != '/')
    {
        np += "/";
    }

    return np + dir;
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
