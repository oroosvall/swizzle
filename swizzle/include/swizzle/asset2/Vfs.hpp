#pragma once

/* Include files */

#include <common/Common.hpp>
#include <swizzle/Api.hpp>
#include <swizzle/core/File.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::asset2
{
    struct VfsInfo
    {
        U32 mFileCount;
        U64 mSize;
    };
} // namespace swizzle::asset2

/* Class Declaration */

namespace swizzle::asset2
{
    class Vfs
    {
    public:
        virtual ~Vfs() {}

        virtual SwBool openStorage(const SwChar* path) = 0;

        virtual VfsInfo vfsInfo() = 0;

        /// <summary>
        /// Add a file to the VFS
        /// </summary>
        /// <param name="logicalPath">The internal VFS path that the resource can be found by</param>
        /// <param name="physicalPath">Location of the file on disk</param>
        virtual void addFile(const SwChar* logicalPath, const SwChar* physicalPath) = 0;

        /// <summary>
        /// Read file content from Vfs
        /// </summary>
        /// <param name="file">Path to the file in the VFS</param>
        /// <returns>Buffer with file data, empty resource if file does not exist</returns>
        virtual common::Resource<IBuffer> readFile(const SwChar* file) = 0;

        /// <summary>
        /// Start packing files to the VFS
        /// </summary>
        virtual void pack() = 0;
    };
} // namespace swizzle::asset2

/* Function Declaration */

namespace swizzle::asset2
{
    /// <summary>
    /// Create a vfs resource and optionally initialize with backing store.
    /// if path is nullptr, then user must open a backing storage.
    /// </summary>
    /// <param name="path">Path to vfs backing file</param>
    /// <returns>Resource to a Vfs instance</returns>
    common::Resource<Vfs> SWIZZLE_API CreateVFSResource(const SwChar* path);
} // namespace swizzle::asset2
