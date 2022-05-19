
/* Include files */

#include "DataBuffer.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle::core
{

    DataBuffer::DataBuffer()
        : mData(nullptr)
        , mSize(0ull)
    {
    }

    DataBuffer::~DataBuffer()
    {
        delete[] mData;
        mSize = 0ull;
    }

    void DataBuffer::clear()
    {
        if (mData)
        {
            delete[] mData;
            mData = nullptr;
            mSize = 0ull;
        }
    }

    void DataBuffer::setData(U8* data, U64 size)
    {
        clear();

        if (data)
        {
            mSize = size;
            mData = new U8[size];
            memcpy(mData, data, size);
        }
    }

    U64 DataBuffer::getSize() const
    {
        return mSize;
    }

    U8* DataBuffer::getData() const
    {
        return mData;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */
