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

        virtual void setBufferData(void* data, SwU64 size, SwU32 stride) = 0;

    };
}

#endif