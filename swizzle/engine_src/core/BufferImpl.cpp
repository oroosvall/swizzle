
/* Include files */

#include "BufferImpl.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle
{
    BufferImpl::BufferImpl(U64 size)
        : mSize(size)
        , mData(nullptr)
    {
        mData = new U8[mSize];
    }

    BufferImpl::~BufferImpl()
    {
        delete[] mData;
        mData = nullptr;
        mSize = 0;
    }

    U64 BufferImpl::size() const
    {
        return mSize;
    }

    U8* BufferImpl::data() const
    {
        return mData;
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */