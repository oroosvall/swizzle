
#pragma once

/* Include files */

#include <swizzle/asset2/Vfs.hpp>
#include <swizzle/core/File.hpp>

#include <string>
#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::asset2
{
#pragma pack(push, 1)
    struct VFSHeader
    {
        SwChar mMagic[4];
        U32 mVersion;
        U32 mFileCount;
        U64 mFileTableOffset;
    };
#pragma pack(pop)

    struct FileInfo
    {
        std::string mFileName;
        std::string mSourcePath;
        U64 mOffset;
        U64 mFileSize;
    };
}

/* Class Declaration */

namespace swizzle::asset2
{
    class VfsImpl : public Vfs
    {
    public:
        VfsImpl();
        virtual ~VfsImpl();

        virtual SwBool openStorage(const SwChar* path) override;

        virtual VfsInfo vfsInfo() override;

        virtual void addFile(const SwChar* logicalPath, const SwChar* physicalPath) override;

        virtual void pack() override;

    private:

        void readHeader();
        void createHeader();
        void writeHeader();
        common::Resource<IBuffer> fileInfoAsBuffer(const FileInfo& fi);

        common::Resource<core::IFile> mVfsFile;
        VFSHeader mHeader;
        std::vector<FileInfo> mFiles;

    };
} // namespace swizzle::asset2

/* Function Declaration */
