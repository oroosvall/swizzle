#include <utils/MemoryBuffer.hpp>

namespace utils
{
    MemoryBuffer::MemoryBuffer(uint32_t size)
        : mArenaBlockSize(size)
        , mNumArenas(0u)
        , mArena(nullptr)
    {
        mArena = createArena();
    }

    MemoryBuffer::~MemoryBuffer()
    {
        destroyArenas();
    }

    void MemoryBuffer::clear()
    {
        destroyArenas();
        mArena = createArena();
    }

    uint8_t* MemoryBuffer::allocMemory(uint32_t size)
    {
        // this will waste memory if there is lots of free space in mArena
        if (size > mArenaBlockSize)
        {
            mArenaBlockSize = size;
            MemoryArena* old = mArena;
            mArena = createArena();
            mArena->mNext = old;
        }
        else if ( size > (mArena->mBufferSize - mArena->mOffset))
        {
            MemoryArena* old = mArena;
            mArena = createArena();
            mArena->mNext = old;
        }
        else
        {
            // do nothing
        }

        uint8_t* memory = (mArena->mBuffer + mArena->mOffset);
        mArena->mOffset += size;

        return memory;
    }

    MemoryBuffer::MemoryArena* MemoryBuffer::createArena()
    {
        MemoryArena* arena = new MemoryArena;
        arena->mBuffer = new uint8_t[mArenaBlockSize];
        arena->mBufferSize = mArenaBlockSize;
        arena->mOffset = 0u;
        arena->mNext = nullptr;
        mNumArenas++;

        return arena;
    }

    void MemoryBuffer::destroyArenas()
    {
        while (mArena)
        {
            MemoryArena* tmp = mArena->mNext;
            delete mArena;
            mArena = tmp;
        }
        mNumArenas = 0u;
    }

} // namespace utils