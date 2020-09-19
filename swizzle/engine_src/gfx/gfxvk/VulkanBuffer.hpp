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

        virtual void setBufferData(void* data, SwU64 size, SwU32 stride) override;

        VkBuffer getBuffer() const { return mBuffer; }

        SwU32 getVertexCount() const { return mVertCount; }

    private:

        void createOrResize(SwU64 newSize);

        const VulkanObjectContainer& mVkObjects;
        const BufferType mType;

        VkBuffer mBuffer;
        VkDeviceMemory mMemory;

        VkDeviceSize mBufferSize;
        SwU32 mStride;
        SwU32 mVertCount;

    };
}

#endif