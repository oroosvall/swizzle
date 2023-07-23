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

    virtual std::string absolutePath(const std::string& path) override;
    virtual std::string getRootPath(const std::string& path) override;
    virtual std::string getWihtoutRootPath(const std::string& path) override;
    virtual std::string getDirectory(const std::string& path) override;
    virtual bool visible(const std::string& path) override;
    virtual bool exists(const std::string& path) override;
    virtual bool isRoot(const std::string& path) override;
    virtual bool isDirectory(const std::string& path) override;
    virtual std::vector<imext::DirectoryItem> getDirectoryItems(const std::string& path, bool onlyDirectories) override;

    virtual void createDir(const std::string& path) override;
    virtual std::string traverseTo(const std::string& path, const std::string& dir) override;

private:

    common::Resource<swizzle::asset2::Vfs> mVfs;
};

/* Function Declaration */
