#ifndef VULKAN_BUFFER_HPP
#define VULKAN_BUFFER_HPP

#include <swizzle/gfx/Buffer.hpp>
#include "vk.hpp"
#include "VulkanMemoryHelper.hpp"

namespace swizzle
{
	class VulkanBuffer : public Buffer
	{
	public:

		VulkanBuffer(VkDevice device, VulkanMemoryHelper& memoryHelper, eBufferType bufferType);
		virtual ~VulkanBuffer();

		virtual eBufferType getBufferType() const override;

		virtual void setBufferData(void* data, const uint64_t size) override;
		virtual uint64_t getBufferSize() const override;

		virtual void* mapBuffer(const uint64_t desiredSize, uint64_t& acutalSize) override;
		virtual void unmapBuffer(void* ptr) override;

	public:

		void createOrResizeBuffer(VkBuffer& buffer, VkDeviceMemory& buffer_memory,
			VkDeviceSize& p_buffer_size, VkDeviceSize new_size);

		VkDevice mDevice;
		VulkanMemoryHelper& mMemoryHelper;
		eBufferType mBufferType;

		VkBuffer mBuffer;
		VkDeviceSize mBufferSize;
		VkDeviceSize mUsedBufferSize;
		VkDeviceMemory mDevMemory;

		void* mMappedMemory;

	};
}

#endif