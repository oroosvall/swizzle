
#pragma once

#ifdef SW_LINUX

/* Include files */

#include <swizzle/core/File.hpp>

#include <mutex>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::core
{
    class LinuxFile : public IFile
    {
    public:
        LinuxFile(const SwChar* path, FileMode mode);
        LinuxFile(const SwWChar* path, FileMode mode);
        virtual ~LinuxFile();

        virtual U64 size() const override;

        virtual U64 getFilePointer() const override;
        virtual void setFilePointer(U64 offset, FilePtrMoveType move) override;

        virtual common::Resource<IBuffer> read(U64 count) override;
        virtual common::Resource<IBuffer> read(U64 offset, U64 count) override;
        virtual void write(common::Resource<IBuffer> data) override;

    private:
        int mFileHandle;

        mutable std::recursive_mutex mFilePtrLock;
    };
}

/* Function Declaration */

#endif