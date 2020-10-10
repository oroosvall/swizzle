#ifndef VULKAN_BUFFER_HPP
#define VULKAN_BUFFER_HPP

#include "vk.hpp"
#include "VulkanObjectContainer.hpp"
#include <swizzle/gfx/Buffer.hpp>

namespace swizzle::gfx
{
    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(const VulkanObjectContainer& vkObjects, BufferType type);
        virtual ~VulkanBuffer();

        virtual void setBufferData(void* data, U64 size, U32 stride) override;

        virtual void* mapMemory(U64 size);
        virtual void unmapMemory();

        VkBuffer getBuffer() const { return mBuffer; }

        U32 getVertexCount() const { return mVertCount; }

    private:

        void createOrResize(U64 newSize);

        const VulkanObjectContainer& mVkObjects;
        const BufferType mType;

        VkBuffer mBuffer;
        VkDeviceMemory mMemory;

        VkDeviceSize mBufferSize;
        U32 mStride;
        U32 mVertCount;

    };
}

#endif