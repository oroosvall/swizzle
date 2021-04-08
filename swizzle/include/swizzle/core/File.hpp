#ifndef FILE_HPP
#define FILE_HPP

/* Include files */

#include <common/Types.hpp>

/* Defines */

/* Typedefs/enums */

namespace swizzle::core
{
    enum class FileType
    {
        None,
        File,
        Directory,
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::core
{
    class File
    {
    public:
        virtual FileType getFileType() const = 0;
        virtual SwBool exists() const = 0;
    };
}

/* Function Declaration */


#endif