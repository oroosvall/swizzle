
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

    struct Entry
    {
        std::string mName;
        SwBool mIsFile;
        U64 mOffset;
        U64 mSize;
        std::vector<Entry> mChildren;
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

    private:

        void readHeader();
        void readTable();
        void readTable(Entry& e, common::Resource<IBuffer> data, U64& offset);

        void createHeader();
        void writeHeader();
        void writeTable();
        U64 writeTable(const Entry& e);

        void writeZero(U64 offset, U64 size);

        /// <summary>
        /// This creates a buffer without the children in it
        /// </summary>
        /// <param name="e">Entry to be converted to buffer</param>
        /// <returns></returns>
        common::Resource<IBuffer> entryAsBuffer(const Entry& e);
        common::Resource<IBuffer> freeInfoAsbuffer(const FreeInfo& fi);

        void removeFileInternal(const SwChar* path);

        Entry& createOrGetEntry(const SwChar* path, SwBool createAsFile);
        SwBool isFile(const Entry& entry) const;
        SwBool exists(const SwChar* path);

        common::Resource<core::IFile> mVfsFile;
        VFSHeader mHeader;
        Entry mRoot;
        std::vector<FreeInfo> mFreeInfos;
    };
} // namespace swizzle::asset2

/* Function Declaration */
