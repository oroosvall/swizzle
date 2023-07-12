
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
    Win32File::Win32File(const SwChar* path)
        :mFileHandle(NULL)
    {
        mFileHandle = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    Win32File::Win32File(const SwWChar* path)
        : mFileHandle(NULL)
    {
        mFileHandle = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    Win32File::~Win32File()
    {
        CloseHandle(mFileHandle);
    }

    U64 Win32File::size() const
    {
        LARGE_INTEGER size{};
        GetFileSizeEx(mFileHandle, &size);
        return size.QuadPart;
    }

    U64 Win32File::getFilePointer() const
    {
        LARGE_INTEGER off{};
        LARGE_INTEGER newPtr{};
        off.QuadPart = 0ull;
        newPtr.QuadPart = 0ull;
        SetFilePointerEx(mFileHandle, off, &newPtr, FILE_CURRENT);
        return newPtr.QuadPart;
    }

    void Win32File::setFilePointer(U64 offset, FilePtrMoveType move)
    {
        LARGE_INTEGER off{};
        off.QuadPart = offset;

        DWORD moveMethod = static_cast<DWORD>(move);

        SetFilePointerEx(mFileHandle, off, NULL, moveMethod);
    }

    common::Resource<IBuffer> Win32File::read(U64 count)
    {
        common::Resource<IBuffer> buf = common::CreateRef<BufferImpl>(count);
        DWORD read = 0u;
        ReadFile(mFileHandle, buf->data(), (DWORD)buf->size(), &read, NULL);

        return buf;
    }

    void Win32File::write(common::Resource<IBuffer> data)
    {
        DWORD written = 0u;
        WriteFile(mFileHandle, data->data(), (DWORD)data->size(), &written, NULL);
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

#endif