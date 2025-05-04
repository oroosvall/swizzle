#pragma once

/* Include files */

#include <ImGuiSwizzleTools.hpp>
#include <swizzle/asset2/Vfs.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class VfsFileSystemInfo : public imext::FileSystemInfo
{
public:
    VfsFileSystemInfo(common::Resource<swizzle::asset2::Vfs> vfs);

    // Inherited via FileSystemInfo
    virtual bool hasLogicalDrives() override;
    virtual std::vector<std::string> getLogicalDrives() override;

    virtual std::filesystem::path absolutePath(const std::filesystem::path& path) override;
    virtual std::filesystem::path getRootPath(const std::filesystem::path& path) override;
    virtual std::filesystem::path getWihtoutRootPath(const std::filesystem::path& path) override;
    virtual std::filesystem::path getDirectory(const std::filesystem::path& path) override;
    virtual bool visible(const std::filesystem::path& path) override;
    virtual bool exists(const std::filesystem::path& path) override;
    virtual bool isRoot(const std::filesystem::path& path) override;
    virtual bool isDirectory(const std::filesystem::path& path) override;
    virtual std::vector<imext::DirectoryItem> getDirectoryItems(const std::filesystem::path& path, bool onlyDirectories) override;

    virtual void createDir(const std::filesystem::path& path) override;
    virtual std::string traverseTo(const std::filesystem::path& path, const std::string& dir) override;

private:

    common::Resource<swizzle::asset2::Vfs> mVfs;
};

/* Function Declaration */
