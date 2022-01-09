
/* Include files */

#include <utils/FrameAllocator.hpp>

#include <memory>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace utils
{
    FrameAllocator::FrameAllocator(size_t byteCount)
        : mMemory(nullptr)
        , mSize(byteCount)
        , mOffset(0ull)
    {
        mMemory = new uint8_t[mSize];
    }

    FrameAllocator::~FrameAllocator()
    {
        delete[] mMemory;
    }

    void FrameAllocator::reset()
    {
        mOffset = 0ull;
    }

    void FrameAllocator::resetAndClear()
    {
        memset(mMemory, 0, mOffset);
        mOffset = 0ull;
    }

    void* FrameAllocator::allocate(size_t size)
    {
        void* mem = nullptr;

        if (mOffset + size <= mSize)
        {
            mem = &mMemory[mOffset];
            mOffset += size;
        }

        return mem;
    }

} // namespace utils

/* Class Protected Function Definition */

/* Class Private Function Definition */
