#ifndef DATA_BUFFER_IMPL_HPP
#define DATA_BUFFER_IMPL_HPP

/* Include files */

#include <swizzle/core/DataBuffer.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::core
{
    class DataBuffer : public IDataBuffer
    {
    public:
        DataBuffer();
        virtual ~DataBuffer();

        virtual void clear() override;

        virtual void setData(U8* data, U64 size) override;

        virtual U64 getSize() const override;
        virtual U8* getData() const override;

    private:
        U8* mData;
        U64 mSize;
    };
}

/* Function Declaration */

#endif
