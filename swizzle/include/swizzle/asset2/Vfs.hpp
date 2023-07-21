#pragma once

/* Include files */

#include <common/Common.hpp>
#include <swizzle/Api.hpp>
#include <swizzle/core/File.hpp>

/* Defines */

/* Typedefs/enums */

namespace swizzle::asset2
{
    enum class VfsReturnCode
    {
        Ok,
        ErrFileNotFound,
        ErrHostFileNotFound,
        ErrHostPermissionDenied,
        ErrAlreadyExists,
        ErrInvalidPath
    };
}

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

        /// <summary>
        /// Open a file to use as vfs
        /// </summary>
        /// <param name="path">Path to the vfs</param>
        /// <returns>VfsReturnCode::Ok, if the file contains a valid vfs, or a 0 byte file
        /// <para>VfsReturnCode::ErrFileNotFound,</para>
        /// </returns>
        virtual SwBool openStorage(const SwChar* path) = 0;

        /// <summary>
        /// Returns statistics about the vfs
        /// </summary>
        /// <returns>Struct containing statistics information</returns>
        virtual VfsInfo vfsInfo() = 0;

        /// <summary>
        /// Add a file to the VFS
        /// </summary>
        /// <param name="logicalPath">The internal VFS path that the resource can be found by</param>
        /// <param name="physicalPath">Location of the file on disk</param>
        /// <returns>VfsReturnCode::Ok, if the file was successfully added
        /// <para>VfsReturnCode::ErrFileNotFound, if the file was not present in the vfs</para>
        /// </returns>
        virtual VfsReturnCode addFile(const SwChar* logicalPath, const SwChar* physicalPath) = 0;

        /// <summary>
        /// Read file content from Vfs
        /// </summary>
        /// <param name="file">Path to the file in the VFS</param>
        /// <returns>Buffer with file data, empty resource if file does not exist</returns>
        virtual common::Resource<IBuffer> readFile(const SwChar* file) = 0;

        /// <summary>
        /// Remove a file from Vfs
        /// </summary>
        /// <param name="path">Path to file in vfs</param>
        /// <returns>VfsReturnCode::Ok, if the file was successfully removed
        /// <para>VfsReturnCode::ErrFileNotFound, if the file could not be found in the vfs</para>
        /// </returns>
        virtual VfsReturnCode removeFile(const SwChar* path) = 0;

        /// <summary>
        /// Check if a file exists in the vfs
        /// </summary>
        /// <param name="path">Path to a file in the vfs</param>
        /// <returns>true if the file exists</returns>
        virtual SwBool exists(const SwChar* path) = 0;

        /// <summary>
        /// Open a file and return a read only file handle
        /// </summary>
        /// <param name="file">Path to a file in the vfs</param>
        /// <returns>File resource on success
        /// <para>Empty resource if file does not exist</para>
        /// </returns>
        virtual common::Resource<core::IFile> openRo(const SwChar* file) = 0;

        /// <summary>
        /// Check if a path is a directory
        /// </summary>
        /// <param name="path">Path to check</param>
        /// <returns>true if the path is a directory</returns>
        virtual SwBool isDirectory(const SwChar* path) = 0;

        /// <summary>
        /// Get the directory name that a file lives in
        /// </summary>
        /// <param name="path">Path to file</param>
        /// <returns>Directory path containing file
        /// <para>nullptr, if path is invalid</para>
        /// </returns>
        virtual const SwChar* getDirectory(const SwChar* path) = 0;

        /// <summary>
        /// Gets number if items in a path
        /// </summary>
        /// <param name="path">path to get number of items in</param>
        /// <returns>Number of items in the path</returns>
        virtual U32 getDirectoryItems(const SwChar* path) = 0;

        /// <summary>
        /// Get a name from the directory with the given path
        /// </summary>
        /// <param name="path">The path to get directory info from</param>
        /// <param name="index">Index to an item</param>
        /// <returns>The name of a item,
        /// <para>nullptr if the index is out of range</para>
        /// </returns>
        virtual const SwChar* getDirectoryItemName(const SwChar* path, U32 index) = 0;

        /// <summary>
        /// Creates a directory for the path
        /// </summary>
        /// <param name="path">Path to create</param>
        /// <returns>VfsReturnCode::Ok, if the directory was created
        /// <para>VfsReturnCode::ErrAlreadyExists, if the directory already exists</para>
        /// <para>VfsReturnCode::ErrInvalidPath, if the path is invalid, e.g contains a name that is a file</para>
        /// </returns>
        virtual VfsReturnCode createDirectory(const SwChar* path) = 0;

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
