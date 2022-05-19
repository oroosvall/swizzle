#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP

/* Include files */

#include <common/Common.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::core
{
    class IDataBuffer
    {
    public:
        virtual ~IDataBuffer() {}

        virtual void clear() = 0;

        virtual void setData(U8* data, U64 size) = 0;

        virtual U64 getSize() const = 0;
        virtual U8* getData() const = 0;
    };
}

/* Function Declaration */

#endif
