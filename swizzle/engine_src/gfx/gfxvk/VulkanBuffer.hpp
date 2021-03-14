#ifndef VULKAN_BUFFER_HPP
#define VULKAN_BUFFER_HPP

#include "backend/Vk.hpp"
#include "backend/VulkanMemory.hpp"
#include "backend/VkContainer.hpp"
#include <swizzle/gfx/Buffer.hpp>

namespace swizzle::gfx
{
    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(const VkContainer vkObjects, BufferType type);
        virtual ~VulkanBuffer();

        virtual void setBufferData(void* data, U64 size, U32 stride) override;

        virtual void* mapMemory(U64 size);
        virtual void unmapMemory();

        VkBuffer getBuffer() const { return mBuffer; }

        U32 getVertexCount() const { return mVertCount; }

    private:

        void createOrResize(U64 newSize);

        const VkContainer mVkObjects;
        const BufferType mType;

        VkBuffer mBuffer;

        vk::VulkanMemory2 mBufferMemory;

        U32 mStride;
        U32 mVertCount;

    };
}

#endif