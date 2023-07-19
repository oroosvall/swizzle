
/* Include files */

#include "VfsImpl.hpp"
#include "core/BufferImpl.hpp"
#include "core/FileUtils.hpp"
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
        , mRoot()
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
        Entry& e = createOrGetEntry(logicalPath, true);

        if (isFile(e))
        {
            common::Resource<core::IFile> srcFile = core::OpenFile(physicalPath);
            const U64 fileSize = srcFile->size();

            if (e.mOffset == 0ull)
            {
                e.mOffset = mHeader.mFileTableOffset;
                e.mSize = fileSize;

                mHeader.mFileCount++;

                mVfsFile->setFilePointer(mHeader.mFileTableOffset, core::FilePtrMoveType::Begin);
                common::Resource<IBuffer> readBuf = srcFile->read(fileSize);
                mVfsFile->write(readBuf);

                mHeader.mFileTableOffset += fileSize;
            }
            else
            {
                FreeInfo freeInfo{};
                freeInfo.mOffset = e.mOffset;
                freeInfo.mSize = e.mSize;

                // zero out old file
                writeZero(e.mOffset, e.mSize);

                // file is larger so replace it
                if (fileSize > e.mSize)
                {
                    e.mOffset = mHeader.mFileTableOffset;
                    e.mSize = fileSize;

                    mVfsFile->setFilePointer(mHeader.mFileTableOffset, core::FilePtrMoveType::Begin);
                    common::Resource<IBuffer> readBuf = srcFile->read(fileSize);
                    mVfsFile->write(readBuf);

                    mHeader.mFileTableOffset += fileSize;
                }
                else
                {
                    e.mSize = fileSize;

                    mVfsFile->setFilePointer(e.mOffset, core::FilePtrMoveType::Begin);
                    common::Resource<IBuffer> readBuf = srcFile->read(fileSize);
                    mVfsFile->write(readBuf);

                    freeInfo.mSize -= fileSize;
                    if (freeInfo.mSize != 0ull)
                    {
                        freeInfo.mOffset = e.mOffset + e.mSize;
                    }
                }

                if (freeInfo.mSize != 0ull)
                {
                    mFreeInfos.push_back(freeInfo);
                }
            }

            writeTable();
            writeHeader();
        }

    }

    common::Resource<IBuffer> VfsImpl::readFile(const SwChar* file)
    {
        common::Resource<IBuffer> data = nullptr;
        if (exists(file))
        {
            Entry e = createOrGetEntry(file, false);
            if (e.mOffset != 0)
            {
                mVfsFile->setFilePointer(e.mOffset, core::FilePtrMoveType::Begin);
                data = mVfsFile->read(e.mSize);
            }
        }
        return data;
    }

    void VfsImpl::removeFile(const SwChar* path)
    {
        Entry e = createOrGetEntry(path, false);

        if (isFile(e))
        {
            mVfsFile->setFilePointer(e.mOffset, core::FilePtrMoveType::Begin);
            auto zeroBuf = common::CreateRef<BufferImpl>(e.mSize);
            memset(zeroBuf->data(), 0u, zeroBuf->size());
            mVfsFile->write(zeroBuf);

            FreeInfo fi{};
            fi.mOffset = e.mOffset;
            fi.mSize = e.mSize;

            mHeader.mFileCount--;
            removeFileInternal(path);

            mFreeInfos.push_back(fi);

            writeTable();
            writeHeader();
        }
        else
        {
            for (const auto& child: e.mChildren)
            {
                std::string p = path;
                p += "/" + child.mName;
                removeFile(p.c_str());
            }
            removeFileInternal(path);
            writeTable();
            writeHeader();
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

        U64 offset = 0ull;
        if(readBuf->size() > 0)
        {
            readTable(mRoot, readBuf, offset);
        }
    }

    void VfsImpl::readTable(Entry& e, common::Resource<IBuffer> data, U64& offset)
    {
        U8* dataPtr = data->data();

        // Read the name
        U32 nameLength = 0ull;
        memcpy(&nameLength, &dataPtr[offset], sizeof(nameLength));
        offset += sizeof(nameLength);
        e.mName.resize(nameLength);
        memcpy(e.mName.data(), &dataPtr[offset], nameLength);
        offset += nameLength;

        // Read isFile
        memcpy(&e.mIsFile, &dataPtr[offset], sizeof(e.mIsFile));
        offset += sizeof(e.mIsFile);

        // Read offset
        memcpy(&e.mOffset, &dataPtr[offset], sizeof(e.mOffset));
        offset += sizeof(e.mOffset);

        // Read size
        memcpy(&e.mSize, &dataPtr[offset], sizeof(e.mSize));
        offset += sizeof(e.mSize);

        U32 childCount = 0ull;
        memcpy(&childCount, &dataPtr[offset], sizeof(childCount));
        offset += sizeof(childCount);

        e.mChildren.resize(childCount);
        for (U32 i = 0; i < childCount; i++)
        {
            readTable(e.mChildren[i], data, offset);
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

        U64 tableSize = writeTable(mRoot);
        mHeader.mFreeTableOffset += tableSize;

        for (const auto& f : mFreeInfos)
        {
            common::Resource<IBuffer> buf = freeInfoAsbuffer(f);
            mVfsFile->write(buf);
        }
    }

    U64 VfsImpl::writeTable(const Entry& e)
    {
        U64 size = 0ull;

        common::Resource<IBuffer> buf = entryAsBuffer(e);
        size += buf->size();
        mVfsFile->write(buf);

        for (const auto& child : e.mChildren)
        {
            size += writeTable(child);
        }

        return size;
    }

    void VfsImpl::writeZero(U64 offset, U64 size)
    {
        mVfsFile->setFilePointer(offset, core::FilePtrMoveType::Begin);
        auto zeroBuf = common::CreateRef<BufferImpl>(size);
        memset(zeroBuf->data(), 0u, zeroBuf->size());
        mVfsFile->write(zeroBuf);
    }

    common::Resource<IBuffer> VfsImpl::entryAsBuffer(const Entry& e)
    {
        U64 size = sizeof(U32) + e.mName.size();
        size += sizeof(e.mIsFile) + sizeof(e.mOffset) + sizeof(e.mSize);
        size += sizeof(static_cast<U32>(e.mChildren.size()));

        common::Resource<IBuffer> buf = common::CreateRef<BufferImpl>(size);

        U8* dataPtr = buf->data();
        U64 offset = 0ull;

        // Write entry name
        U32 nameSize = static_cast<U32>(e.mName.size());
        memcpy(&dataPtr[offset], &nameSize, sizeof(nameSize));
        offset += sizeof(nameSize);
        memcpy(&dataPtr[offset], e.mName.data(), nameSize);
        offset += nameSize;

        // Write isFile
        memcpy(&dataPtr[offset], &e.mIsFile, sizeof(e.mIsFile));
        offset += sizeof(e.mIsFile);

        // Write offset
        memcpy(&dataPtr[offset], &e.mOffset, sizeof(e.mOffset));
        offset += sizeof(e.mOffset);

        // Write size
        memcpy(&dataPtr[offset], &e.mSize, sizeof(e.mSize));
        offset += sizeof(e.mSize);

        // Write number of children
        U32 childCount = static_cast<U32>(e.mChildren.size());
        memcpy(&dataPtr[offset], &childCount, sizeof(childCount));

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

    // TODO: remove all children
    void VfsImpl::removeFileInternal(const SwChar* path)
    {
        std::string strPath = path;
        std::vector<std::string> paths = core::getPaths(strPath);
        std::string name = paths.back();
        paths.pop_back();

        Entry* entryPtr = &mRoot;
        for (const auto& p : paths)
        {

            for (size_t i = 0ull; i < entryPtr->mChildren.size(); ++i)
            {
                if (entryPtr->mChildren[i].mName == p)
                {
                    entryPtr = &entryPtr->mChildren[i];
                    break;
                }
            }
        }

        std::vector<Entry>::iterator toDel;
        for (auto it = entryPtr->mChildren.begin(); it != entryPtr->mChildren.end(); it++)
        {
            if (it->mName == name)
            {
                toDel = it;
                break;
            }
        }
        entryPtr->mChildren.erase(toDel);

    }

    Entry& VfsImpl::createOrGetEntry(const SwChar* path, SwBool createAsFile)
    {
        std::string strPath = path;
        std::vector<std::string> paths = core::getPaths(strPath);

        Entry* entryPtr = &mRoot;
        for (const auto& p : paths)
        {
            SwBool createNode = true;

            for (size_t i = 0ull; i < entryPtr->mChildren.size(); ++i)
            {
                if (entryPtr->mChildren[i].mName == p)
                {
                    createNode = false;
                    entryPtr = &entryPtr->mChildren[i];
                    break;
                }
            }

            if (createNode)
            {
                Entry newNode{};
                newNode.mName = p;
                entryPtr->mChildren.push_back(newNode);
                entryPtr = &entryPtr->mChildren.back();
            }
        }

        if (createAsFile && (entryPtr->mChildren.empty()) && (!entryPtr->mIsFile))
        {
            entryPtr->mIsFile = true;
        }

        return *entryPtr;
    }

    SwBool VfsImpl::isFile(const Entry& entry) const
    {
        return entry.mIsFile;
    }

    SwBool VfsImpl::exists(const SwChar* path)
    {
        SwBool exists = false;
        std::string strPath = path;
        std::vector<std::string> paths = core::getPaths(strPath);

        Entry* entryPtr = &mRoot;
        for (const auto& p : paths)
        {
            for (size_t i = 0ull; i < entryPtr->mChildren.size(); ++i)
            {
                if (entryPtr->mChildren[i].mName == p)
                {
                    entryPtr = &entryPtr->mChildren[i];
                    break;
                }
            }
        }

        if (entryPtr->mName == paths.back())
        {
            exists = true;
        }

        return exists;
    }

} // namespace swizzle::asset2