
#pragma once

#ifdef SW_WINDOWS

/* Include files */

#include <swizzle/core/File.hpp>
#include <Windows.h>

#include <mutex>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::core
{
    class Win32File : public IFile
    {
    public:
        Win32File(const SwChar* path, FileMode mode);
        Win32File(const SwWChar* path, FileMode mode);
        virtual ~Win32File();

        virtual U64 size() const override;

        virtual U64 getFilePointer() const override;
        virtual void setFilePointer(U64 offset, FilePtrMoveType move) override;

        virtual common::Resource<IBuffer> read(U64 count) override;
        virtual common::Resource<IBuffer> read(U64 offset, U64 count) override;
        virtual void write(common::Resource<IBuffer> data) override;

    private:
        HANDLE mFileHandle;

        mutable std::recursive_mutex mFilePtrLock;
    };
}

/* Function Declaration */

#endif