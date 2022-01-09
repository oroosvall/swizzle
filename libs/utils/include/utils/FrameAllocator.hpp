#ifndef FRAME_ALLOCATOR_HPP
#define FRAME_ALLOCATOR_HPP

/* Include files */

#include <cstdint>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace utils
{
    class FrameAllocator
    {
    public:
        FrameAllocator(size_t byteCount);
        ~FrameAllocator();

        void reset();
        void resetAndClear();
        void* allocate(size_t size);

    private:

        uint8_t* mMemory;
        size_t mSize;
        size_t mOffset;

    };
}

/* Function Declaration */

#endif
