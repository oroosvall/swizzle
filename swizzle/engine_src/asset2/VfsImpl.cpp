
/* Include files */

#include "VfsImpl.hpp"
#include "core/BufferImpl.hpp"
#include <swizzle/core/Platform.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset2
{
    common::Resource<Vfs> CreateVFSResource(const SwChar* path)
    {
        auto vfs = common::CreateRef<VfsImpl>();
        if (path)
        {
            vfs->openStorage(path);
        }
        return vfs;
    }
} // namespace swizzle::asset2

/* Class Public Function Definition */

namespace swizzle::asset2
{
    VfsImpl::VfsImpl()
        : mVfsFile(nullptr)
        , mHeader()
        , mFiles()
    {
    }

    VfsImpl::~VfsImpl()
    {
        mVfsFile.reset();
    }

    SwBool VfsImpl::openStorage(const SwChar* path)
    {
        mVfsFile = core::OpenFile(path);
        if (mVfsFile->size() != 0)
        {
            readHeader();
        }
        else
        {
            createHeader();
            writeHeader();
        }
        return true;
    }

    VfsInfo VfsImpl::vfsInfo()
    {
        return {mHeader.mFileCount};
    }

    void VfsImpl::addFile(const SwChar* logicalPath, const SwChar* physicalPath)
    {
        FileInfo info{};

        common::Resource<core::IFile> srcFile = core::OpenFile(physicalPath);

        info.mFileName = logicalPath;
        info.mSourcePath = physicalPath;
        info.mOffset = mHeader.mFileTableOffset;
        info.mFileSize = srcFile->size();

        mHeader.mFileCount++;
        mHeader.mFileTableOffset += info.mFileSize;

        mFiles.push_back(info);
    }

    void VfsImpl::pack()
    {
        mVfsFile->setFilePointer(0ull, core::FilePtrMoveType::Begin);
        writeHeader();
        std::vector<common::Resource<IBuffer>> buffers;
        for (const auto& f : mFiles)
        {
            common::Resource<core::IFile> srcFile = core::OpenFile(f.mSourcePath.c_str());
            common::Resource<IBuffer> read = srcFile->read(f.mFileSize);
            mVfsFile->write(read);

            buffers.push_back(fileInfoAsBuffer(f));
        }

        for (const auto& b : buffers)
        {
            mVfsFile->write(b);
        }
    }

} // namespace swizzle::asset2

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace swizzle::asset2
{
    void VfsImpl::readHeader()
    {
        common::Resource<IBuffer> data = mVfsFile->read(sizeof(mHeader));
        if (data->size() == sizeof(mHeader))
        {
            memcpy(&mHeader, data->data(), sizeof(mHeader));
        }
    }

    void VfsImpl::createHeader()
    {
        mHeader.mMagic[0] = 'V';
        mHeader.mMagic[1] = 'f';
        mHeader.mMagic[2] = 's';
        mHeader.mMagic[3] = '\0';

        mHeader.mVersion = 0;
        mHeader.mFileCount = 0;
        mHeader.mFileTableOffset = sizeof(mHeader);
    }

    void VfsImpl::writeHeader()
    {
        U64 fp = mVfsFile->getFilePointer();
        mVfsFile->setFilePointer(0ull, core::FilePtrMoveType::Begin);
        auto data = common::CreateRef<RawBuffer>((U8*)&mHeader, sizeof(mHeader));
        mVfsFile->write(data);
        if (fp != 0)
        {
            mVfsFile->setFilePointer(fp, core::FilePtrMoveType::Begin);
        }
    }

    common::Resource<IBuffer> VfsImpl::fileInfoAsBuffer(const FileInfo& fi)
    {
        U64 size = sizeof(fi.mFileSize) + sizeof(fi.mOffset);
        size += sizeof(U32) + sizeof(U32);
        size += fi.mFileName.size() + fi.mSourcePath.size();

        common::Resource<IBuffer> buf = common::CreateRef<BufferImpl>(size);

        U8* dataPtr = buf->data();
        U64 offset = 0ull;

        // Write local file name
        U32 nameSize = static_cast<U32>(fi.mFileName.size());
        memcpy(&dataPtr[offset], &nameSize, sizeof(nameSize));
        offset += sizeof(nameSize);
        memcpy(&dataPtr[offset], fi.mFileName.data(), nameSize);
        offset += nameSize;

        // Write source file name
        nameSize = static_cast<U32>(fi.mSourcePath.size());
        memcpy(&dataPtr[offset], &nameSize, sizeof(nameSize));
        offset += sizeof(nameSize);
        memcpy(&dataPtr[offset], fi.mSourcePath.data(), nameSize);
        offset += nameSize;

        // Write file offset
        memcpy(&dataPtr[offset], &fi.mOffset, sizeof(fi.mOffset));
        offset += sizeof(fi.mOffset);

        // Write file size
        memcpy(&dataPtr[offset], &nameSize, sizeof(fi.mFileSize));
        offset += sizeof(fi.mFileSize);

        return buf;
    }

} // namespace swizzle::asset2