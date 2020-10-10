#ifndef BUFFER_HPP
#define BUFFER_HPP

namespace swizzle::gfx
{

    enum class BufferType
    {
        Vertex,
        Index
    };

    class Buffer
    {
    public:

        virtual ~Buffer() {}

        virtual void setBufferData(void* data, U64 size, U32 stride) = 0;

        virtual void* mapMemory(U64 size) = 0;
        virtual void unmapMemory() = 0;
    };
}

#endif