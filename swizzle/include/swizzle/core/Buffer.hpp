
#pragma once

/* Include files */

#include <common/Common.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle
{

    class IBuffer
    {
    public:
        virtual ~IBuffer() {}

        virtual U64 size() const = 0;
        virtual U8* data() const = 0;
    };

    /// <summary>
    /// Wrapper for raw data types.
    /// This does not become data owner
    /// </summary>
    class RawBuffer : public IBuffer
    {
    public:
        RawBuffer(U8* data, U64 size)
            : mData(data)
            , mSize(size)
        {
        }
        virtual ~RawBuffer() {}

        virtual U64 size() const override
        {
            return mSize;
        }

        virtual U8* data() const override
        {
            return mData;
        }

    private:
        U8* mData;
        U64 mSize;
    };

} // namespace swizzle

/* Function Declaration */
