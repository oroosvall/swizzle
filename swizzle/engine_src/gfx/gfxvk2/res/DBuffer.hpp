#ifndef D_BUFFER_HPP
#define D_BUFFER_HPP

/* Include files */

#include <swizzle/gfx/Buffer.hpp>

#include "_fwDecl.hpp"
#include "../backend/Vk.hpp"

#include "VkResource.hpp"
#include "DeviceMemory.hpp"

#include <memory>
#include <vector>
#include <set>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class DBuffer : public swizzle::gfx::Buffer
    {
    public:
        DBuffer(common::Resource<Device> device, swizzle::gfx::BufferType type);
        virtual ~DBuffer();

        virtual void setBufferData(void* data, U64 size, U32 stride) override;
        virtual U64 getRemainingSize() const override;

        virtual void setStride(U32 stride) override;

        virtual void* mapMemory(U64 size) override;
        virtual void unmapMemory() override;

        common::Resource<VkResource<VkBuffer>>& getBuffer();

        U32 getVertexCount();

        void upload();

    private:

        void createOrResize(U64 newSize);

        common::Resource<Device> mDevice;
        swizzle::gfx::BufferType mType;

        common::Resource<VkResource<VkBuffer>> mBuffer;
        common::Resource<DeviceMemory> mMemory;

        U32 mStride;
        U32 mVertCount;
        U64 mUsedSize;
        U64 mBufferSize;
    };
}

/* Function Declaration */

#endif