#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <common/Common.hpp>

namespace swizzle::gfx
{

    enum class BufferType
    {
        Vertex,
        Index,
        UniformBuffer,
    };

    class Buffer
    {
    public:

        virtual void setBufferData(void* data, U64 size, U32 stride) = 0;
        virtual U64 getRemainingSize() const = 0;
        virtual U64 getSize() const = 0;

        virtual void setStride(U32 stride) = 0;

        virtual void* mapMemory(U64 size) = 0;
        virtual void unmapMemory() = 0;

    protected:
        virtual ~Buffer() {}

    };
}

#endif