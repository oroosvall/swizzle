
#ifdef SW_WINDOWS

/* Include files */

#include "Win32File.hpp"
#include "core/BufferImpl.hpp"

#include <Windows.h>

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
    Win32File::Win32File(const SwChar* path, FileMode mode)
        : mFileHandle(NULL)
        , mFilePtrLock()
    {
        DWORD flags = 0;

        switch (mode)
        {
        case swizzle::core::FileMode::Write:
            flags = GENERIC_WRITE;
            break;
        case swizzle::core::FileMode::ReadWrite:
            flags = GENERIC_READ | GENERIC_WRITE;
            break;
        case swizzle::core::FileMode::Read:
        default:
            flags = GENERIC_READ;
            break;
        }

        mFileHandle =
            CreateFileA(path, flags, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    Win32File::Win32File(const SwWChar* path, FileMode mode)
        : mFileHandle(NULL)
    {
        DWORD flags = 0;

        switch (mode)
        {
        case swizzle::core::FileMode::Write:
            flags = GENERIC_WRITE;
            break;
        case swizzle::core::FileMode::ReadWrite:
            flags = GENERIC_READ | GENERIC_WRITE;
            break;
        case swizzle::core::FileMode::Read:
        default:
            flags = GENERIC_READ;
            break;
        }

        mFileHandle =
            CreateFileW(path, flags, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    Win32File::~Win32File()
    {
        CloseHandle(mFileHandle);
    }

    U64 Win32File::size() const
    {
        std::lock_guard lock(mFilePtrLock);
        LARGE_INTEGER size{};
        GetFileSizeEx(mFileHandle, &size);
        return size.QuadPart;
    }

    U64 Win32File::getFilePointer() const
    {
        std::lock_guard lock(mFilePtrLock);

        LARGE_INTEGER off{};
        LARGE_INTEGER newPtr{};
        off.QuadPart = 0ull;
        newPtr.QuadPart = 0ull;
        SetFilePointerEx(mFileHandle, off, &newPtr, FILE_CURRENT);
        return newPtr.QuadPart;
    }

    void Win32File::setFilePointer(U64 offset, FilePtrMoveType move)
    {
        std::lock_guard lock(mFilePtrLock);

        LARGE_INTEGER off{};
        off.QuadPart = offset;

        DWORD moveMethod = static_cast<DWORD>(move);

        SetFilePointerEx(mFileHandle, off, NULL, moveMethod);
    }

    common::Resource<IBuffer> Win32File::read(U64 count)
    {
        std::lock_guard lock(mFilePtrLock);

        common::Resource<IBuffer> buf = common::CreateRef<BufferImpl>(count);
        DWORD read = 0u;
        ReadFile(mFileHandle, buf->data(), (DWORD)buf->size(), &read, NULL);

        return buf;
    }

    common::Resource<IBuffer> Win32File::read(U64 offset, U64 count)
    {
        std::lock_guard lock(mFilePtrLock);

        setFilePointer(offset, FilePtrMoveType::Begin);

        common::Resource<IBuffer> buf = common::CreateRef<BufferImpl>(count);
        DWORD read = 0u;
        ReadFile(mFileHandle, buf->data(), (DWORD)buf->size(), &read, NULL);

        return buf;
    }

    void Win32File::write(common::Resource<IBuffer> data)
    {
        std::lock_guard lock(mFilePtrLock);

        DWORD written = 0u;
        WriteFile(mFileHandle, data->data(), (DWORD)data->size(), &written, NULL);
    }
} // namespace swizzle::core

/* Class Protected Function Definition */

/* Class Private Function Definition */

#endif