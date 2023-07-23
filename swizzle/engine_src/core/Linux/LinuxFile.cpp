
#ifdef SW_LINUX

/* Include files */

#include "LinuxFile.hpp"
#include "core/BufferImpl.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <iconv.h>

#include <string>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle::core
{
    LinuxFile::LinuxFile(const SwChar* path, FileMode mode)
        : mFileHandle(-1)
        , mFilePtrLock()
    {
        S32 flags = 0;

        switch (mode)
        {
        case swizzle::core::FileMode::Write:
            flags = O_WRONLY;
            break;
        case swizzle::core::FileMode::ReadWrite:
            flags = O_RDWR;
            break;
        case swizzle::core::FileMode::Read:
        default:
            flags = O_RDONLY;
            break;
        }

        mFileHandle = open(path, flags);
    }

    LinuxFile::LinuxFile(const SwWChar* path, FileMode mode)
        : mFileHandle(-1)
    {
        // TODO convert wchar to utf8 string
        S32 flags = 0;

        switch (mode)
        {
        case swizzle::core::FileMode::Write:
            flags = O_WRONLY;
            break;
        case swizzle::core::FileMode::ReadWrite:
            flags = O_RDWR;
            break;
        case swizzle::core::FileMode::Read:
        default:
            flags = O_RDONLY;
            break;
        }

        U64 len = wcslen(path);
        U64 len2 = len;
        SwChar* p = new SwChar[len];

        iconv_t conv = iconv_open("UTF-8", "UTF-16");
        iconv(conv, (char**)&path, &len, &p, &len2);
        iconv_close(conv);

        mFileHandle = open(p, flags);


        delete [] p;
    }

    LinuxFile::~LinuxFile()
    {
        close(mFileHandle);
    }

    U64 LinuxFile::size() const
    {
        std::lock_guard lock(mFilePtrLock);
        struct stat st;
        fstat(mFileHandle, &st);
        return st.st_size;
    }

    U64 LinuxFile::getFilePointer() const
    {
        std::lock_guard lock(mFilePtrLock);

        U64 off = lseek64(mFileHandle, 0U, SEEK_CUR);
        return off;
    }

    void LinuxFile::setFilePointer(U64 offset, FilePtrMoveType move)
    {
        std::lock_guard lock(mFilePtrLock);

        int mv = 0;
        switch (move)
        {
        case FilePtrMoveType::Begin:
            mv = SEEK_SET;
            break;
        case FilePtrMoveType::Current:
            mv = SEEK_CUR;
            break;
        case FilePtrMoveType::End:
            mv = SEEK_END;
            break;
        default:
            break;
        }

        lseek64(mFileHandle, offset, mv);
    }

    common::Resource<IBuffer> LinuxFile::read(U64 count)
    {
        std::lock_guard lock(mFilePtrLock);

        common::Resource<IBuffer> buf = common::CreateRef<BufferImpl>(count);
        auto r = ::read(mFileHandle, (void*)buf->data(), buf->size());
        r=r;

        return buf;
    }

    common::Resource<IBuffer> LinuxFile::read(U64 offset, U64 count)
    {
        std::lock_guard lock(mFilePtrLock);

        setFilePointer(offset, FilePtrMoveType::Begin);

        common::Resource<IBuffer> buf = common::CreateRef<BufferImpl>(count);
        auto r = ::read(mFileHandle, (void*)buf->data(), buf->size());
        r=r;
        return buf;
    }

    void LinuxFile::write(common::Resource<IBuffer> data)
    {
        std::lock_guard lock(mFilePtrLock);

        auto r = ::write(mFileHandle, (void*)data->data(), data->size());
        r=r;
    }
} // namespace swizzle::core

/* Class Protected Function Definition */

/* Class Private Function Definition */

#endif