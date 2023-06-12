
#pragma once

/* Include files */

#include <swizzle/core/Buffer.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle
{
    class BufferImpl : public IBuffer
    {
    public:
        BufferImpl(U64 size);
        virtual ~BufferImpl();

        virtual U64 size() const override;
        virtual U8* data() const override;

    private:

        U64 mSize;
        U8* mData;
    };
}

/* Function Declaration */
