
#ifndef MEMORY_BUFFER_HPP
#define MEMORY_BUFFER_HPP

#include <cstdint>

namespace utils
{
    class MemoryBuffer
    {
    public:
        MemoryBuffer(uint32_t size);
        ~MemoryBuffer();

        void clear();

        uint8_t* allocMemory(uint32_t size);

    private:

        struct MemoryArena
        {
            uint8_t* mBuffer;
            uint32_t mBufferSize;

            uint32_t mOffset;

            MemoryArena* mNext;
        };

        MemoryArena* createArena();
        void destroyArenas();

        uint32_t mArenaBlockSize;
        uint32_t mNumArenas;
        MemoryArena* mArena;

    };

} // namespace utils

#endif