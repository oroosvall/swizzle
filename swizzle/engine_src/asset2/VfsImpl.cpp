
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
        , mFreeInfos()
    {
    }

    VfsImpl::~VfsImpl()
    {
        U64 size = mVfsFile->size();
        mVfsFile->setFilePointer(size, core::FilePtrMoveType::Begin);
        mVfsFile.reset();
    }

    SwBool VfsImpl::openStorage(const SwChar* path)
    {
        mVfsFile = core::OpenFile(path);
        if (mVfsFile->size() != 0)
        {
            readHeader();
            readTable();
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
        return {mHeader.mFileCount, mVfsFile->size()};
    }

    void VfsImpl::addFile(const SwChar* logicalPath, const SwChar* physicalPath)
    {
        FileInfo found = findFile(logicalPath);
        // Use offset to determine if file already exists in vfs since a file could technically be 0 bytes
        if (found.mOffset == 0ull)
        {
            mVfsFile->setFilePointer(mHeader.mFileTableOffset, core::FilePtrMoveType::Begin);
            FileInfo info{};

            common::Resource<core::IFile> srcFile = core::OpenFile(physicalPath);

            info.mFileName = logicalPath;
            info.mSourcePath = physicalPath;
            info.mOffset = mHeader.mFileTableOffset;
            info.mFileSize = srcFile->size();

            mHeader.mFileCount++;
            mHeader.mFileTableOffset += info.mFileSize;

            mFiles.push_back(info);

            common::Resource<IBuffer> readBuf = srcFile->read(info.mFileSize);
            mVfsFile->write(readBuf);

            writeTable();
            writeHeader();
        }
        else
        {
            common::Resource<core::IFile> srcFile = core::OpenFile(physicalPath);
            FreeInfo freeInfo{};
            freeInfo.mOffset = found.mOffset;
            freeInfo.mSize = found.mFileSize;

            // file is larger so replace it
            if (srcFile->size() > found.mFileSize)
            {
                mVfsFile->setFilePointer(found.mOffset, core::FilePtrMoveType::Begin);
                auto zeroBuf = common::CreateRef<BufferImpl>(found.mFileSize);
                memset(zeroBuf->data(), 0u, zeroBuf->size());
                mVfsFile->write(zeroBuf);
                zeroBuf.reset();

                found.mSourcePath = physicalPath;
                found.mOffset = mHeader.mFileTableOffset;
                found.mFileSize = srcFile->size();

                mVfsFile->setFilePointer(mHeader.mFileTableOffset, core::FilePtrMoveType::Begin);

                common::Resource<IBuffer> readBuf = srcFile->read(found.mFileSize);
                mVfsFile->write(readBuf);

                mHeader.mFileTableOffset += found.mFileSize;

                updateFile(found);
            }
            // Same size or smaller
            else
            {
                mVfsFile->setFilePointer(found.mOffset, core::FilePtrMoveType::Begin);
                auto zeroBuf = common::CreateRef<BufferImpl>(found.mFileSize);
                memset(zeroBuf->data(), 0u, zeroBuf->size());
                mVfsFile->write(zeroBuf);
                zeroBuf.reset();

                found.mSourcePath = physicalPath;
                found.mFileSize = srcFile->size();

                mVfsFile->setFilePointer(found.mOffset, core::FilePtrMoveType::Begin);
                common::Resource<IBuffer> readBuf = srcFile->read(found.mFileSize);
                mVfsFile->write(readBuf);

                freeInfo.mSize -= found.mFileSize;
                if (freeInfo.mSize != 0ull)
                {
                    freeInfo.mOffset = found.mOffset + found.mFileSize;
                }
            }

            if (freeInfo.mSize != 0ull)
            {
                mFreeInfos.push_back(freeInfo);
            }

            writeTable();
            writeHeader();
        }
    }

    common::Resource<IBuffer> VfsImpl::readFile(const SwChar* file)
    {
        FileInfo found = findFile(file);
        common::Resource<IBuffer> data = nullptr;
        if (found.mOffset != 0)
        {
            mVfsFile->setFilePointer(found.mOffset, core::FilePtrMoveType::Begin);
            data = mVfsFile->read(found.mFileSize);
        }

        return data;
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

    void VfsImpl::readTable()
    {
        mVfsFile->setFilePointer(mHeader.mFileTableOffset, core::FilePtrMoveType::Begin);
        common::Resource<IBuffer> readBuf = mVfsFile->read(mHeader.mFreeTableOffset - mHeader.mFileTableOffset);

        U8* data = readBuf->data();
        U64 offset = 0ull;
        for (U32 i = 0ull; i < mHeader.mFileCount; ++i)
        {
            FileInfo fi{};

            // Read fileName
            U32 nameLength = 0ul;
            memcpy(&nameLength, &data[offset], sizeof(nameLength));
            offset += sizeof(nameLength);
            fi.mFileName.resize(nameLength);
            memcpy(fi.mFileName.data(), &data[offset], nameLength);
            offset += nameLength;

            // Read source filePath
            nameLength = 0ul;
            memcpy(&nameLength, &data[offset], sizeof(nameLength));
            offset += sizeof(nameLength);
            fi.mSourcePath.resize(nameLength);
            memcpy(fi.mSourcePath.data(), &data[offset], nameLength);
            offset += nameLength;

            // Read offset
            memcpy(&fi.mOffset, &data[offset], sizeof(fi.mOffset));
            offset += sizeof(fi.mOffset);

            // Read size
            memcpy(&fi.mFileSize, &data[offset], sizeof(fi.mFileSize));
            offset += sizeof(fi.mFileSize);

            mFiles.push_back(fi);
        }
    }

    void VfsImpl::createHeader()
    {
        mHeader.mMagic[0] = 'V';
        mHeader.mMagic[1] = 'f';
        mHeader.mMagic[2] = 's';
        mHeader.mMagic[3] = '\0';

        mHeader.mHeaderSize = sizeof(mHeader);
        mHeader.mVersion = 0;
        mHeader.mFileCount = 0;
        mHeader.mFreeCount = 0;
        mHeader.mFileTableOffset = sizeof(mHeader);
        mHeader.mFreeTableOffset = sizeof(mHeader);
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

    void VfsImpl::writeTable()
    {
        mVfsFile->setFilePointer(mHeader.mFileTableOffset, core::FilePtrMoveType::Begin);
        mHeader.mFreeTableOffset = mHeader.mFileTableOffset;
        for (const auto& f : mFiles)
        {
            common::Resource<IBuffer> buf = fileInfoAsBuffer(f);
            mHeader.mFreeTableOffset += buf->size();
            mVfsFile->write(buf);
        }

        for (const auto& f : mFreeInfos)
        {
            common::Resource<IBuffer> buf = freeInfoAsbuffer(f);
            mVfsFile->write(buf);
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
        memcpy(&dataPtr[offset], &fi.mFileSize, sizeof(fi.mFileSize));
        offset += sizeof(fi.mFileSize);

        return buf;
    }

    common::Resource<IBuffer> VfsImpl::freeInfoAsbuffer(const FreeInfo& fi)
    {
        U64 size = sizeof(fi.mSize) + sizeof(fi.mOffset);
        common::Resource<IBuffer> buf = common::CreateRef<BufferImpl>(size);

        U8* dataPtr = buf->data();
        U64 offset = 0ull;

        // Write file offset
        memcpy(&dataPtr[offset], &fi.mOffset, sizeof(fi.mOffset));
        offset += sizeof(fi.mOffset);

        // Write file size
        memcpy(&dataPtr[offset], &fi.mSize, sizeof(fi.mSize));
        offset += sizeof(fi.mSize);

        return buf;
    }

    FileInfo VfsImpl::findFile(const SwChar* path)
    {
        FileInfo fi{};
        std::string toFind = path;

        for (const auto& f : mFiles)
        {
            if (f.mFileName == toFind)
            {
                fi = f;
                break;
            }
        }

        return fi;
    }

    void VfsImpl::updateFile(const FileInfo& fi)
    {
        for (auto& f : mFiles)
        {
            if (f.mFileName == fi.mFileName)
            {
                f = fi;
                break;
            }
        }
    }

} // namespace swizzle::asset2