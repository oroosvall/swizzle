#ifndef VULKAN_BUFFER_HPP
#define VULKAN_BUFFER_HPP


/* Include files */

#include "backend/Vk.hpp"
#include "backend/VulkanMemory.hpp"
#include "backend/VkContainer.hpp"
#include <swizzle/gfx/Buffer.hpp>


/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::gfx
{
    class VkBufferInt
    {
    public:
        std::shared_ptr<LogicalDevice> mDevice;
        VkBuffer mBuffer;
        VkBufferInt(std::shared_ptr<LogicalDevice> device, VkBuffer buffer)
            : mDevice(device)
            , mBuffer(buffer)
        {
        }

        virtual ~VkBufferInt()
        {
            mDevice->destroyBuffer(mBuffer);
        }
    };
}

/* Class Declaration */

namespace swizzle::gfx
{
    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(const VkContainer vkObjects, BufferType type);
        virtual ~VulkanBuffer();

        virtual void setBufferData(void* data, U64 size, U32 stride) override;
        virtual U64 getRemainingSize() const override;

        virtual void* mapMemory(U64 size);
        virtual void unmapMemory();

        std::shared_ptr<VkBufferInt> getBuffer() const { return mBuffer; }

        U32 getVertexCount() const { return mVertCount; }

    private:

        void createOrResize(U64 newSize);

        const VkContainer mVkObjects;
        const BufferType mType;

        std::shared_ptr<VkBufferInt> mBuffer;

        vk::VulkanMemory mBufferMemory;

        U32 mStride;
        U32 mVertCount;
        VkDeviceSize mUsedSize;

    };
}

/* Function Declaration */

#endif