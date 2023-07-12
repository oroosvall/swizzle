#pragma once

/* Include files */

#include <common/Common.hpp>
#include <swizzle/Api.hpp>
#include <swizzle/core/Buffer.hpp>

/* Defines */

/* Typedefs/enums */

namespace swizzle::core
{
    enum class FilePtrMoveType
    {
        Begin = 0,
        Current = 1,
        End = 2
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::core
{

    class IFileMapView
    {
    public:
        virtual ~IFileMapView() { }
    };

    class IFileMap
    {
    public:
        virtual ~IFileMap()
        {
        }

        virtual U64 size() const = 0;

        virtual common::Resource<IFileMapView> openView(U64 offset, U64 size) = 0;

    };


    /// <summary>
    /// This class is an interface to a os backed file handle
    /// that have additional functions such as memory mapping
    /// </summary>
    class IFile
    {
    public:
        virtual ~IFile() {}

        virtual U64 size() const = 0;

        virtual U64 getFilePointer() const = 0;
        virtual void setFilePointer(U64 offset, FilePtrMoveType move) = 0;

        virtual common::Resource<IBuffer> read(U64 count) = 0;
        virtual void write(common::Resource<IBuffer> data) = 0;

        virtual common::Resource<IFileMap> mapFile(U64 size) = 0;
    };

} // namespace swizzle::core

/* Function Declaration */
