
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
        U16 mHeaderSize;
        U16 mVersion;
        U32 mFileCount;
        U32 mFreeCount;
        U64 mFileTableOffset;
        U64 mFreeTableOffset;
    };
#pragma pack(pop)

    struct FileInfo
    {
        std::string mFileName;
        std::string mSourcePath;
        U64 mOffset;
        U64 mFileSize;
    };
    struct FreeInfo
    {
        U64 mOffset;
        U64 mSize;
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

        virtual common::Resource<IBuffer> readFile(const SwChar* file) override;

        virtual void removeFile(const SwChar* path) override;

        virtual void pack() override;

    private:

        void readHeader();
        void readTable();

        void createHeader();
        void writeHeader();
        void writeTable();

        common::Resource<IBuffer> fileInfoAsBuffer(const FileInfo& fi);
        common::Resource<IBuffer> freeInfoAsbuffer(const FreeInfo& fi);

        FileInfo findFile(const SwChar* path);
        void updateFile(const FileInfo& fi);
        void removeFile(const FileInfo& fi);

        common::Resource<core::IFile> mVfsFile;
        VFSHeader mHeader;
        std::vector<FileInfo> mFiles;
        std::vector<FreeInfo> mFreeInfos;
    };
} // namespace swizzle::asset2

/* Function Declaration */
