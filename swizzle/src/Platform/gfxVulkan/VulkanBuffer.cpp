
#include "VulkanBuffer.hpp"


namespace swizzle
{

	static VkDeviceSize g_BufferMemoryAlignment = 256;

	VulkanBuffer::VulkanBuffer(VkDevice device, VulkanMemoryHelper& memoryHelper, eBufferType bufferType)
		: mDevice(device)
		, mMemoryHelper(memoryHelper)
		, mBufferType(bufferType)
		, mBuffer(VK_NULL_HANDLE)
		, mBufferSize(0U)
		, mUsedBufferSize(0U)
		, mDevMemory(VK_NULL_HANDLE)
		, mMappedMemory(nullptr)
	{
	}

	VulkanBuffer::~VulkanBuffer()
	{
		if (mBuffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(mDevice, mBuffer, nullptr);
			mBuffer = VK_NULL_HANDLE;
		}
		if (mDevMemory)
		{
			vkFreeMemory(mDevice, mDevMemory, nullptr);
			mDevMemory = VK_NULL_HANDLE;
		}
	}

	eBufferType VulkanBuffer::getBufferType() const
	{
		return mBufferType;
	}

	void VulkanBuffer::setBufferData(void* data, const uint64_t size)
	{
		createOrResizeBuffer(mBuffer, mDevMemory, mBufferSize, size);

		void* mappedPtr = nullptr;
		vkMapMemory(mDevice, mDevMemory, 0, mBufferSize, 0, &mappedPtr);
		uint64_t minSize = mBufferSize > size ? size : mBufferSize;
		mUsedBufferSize = minSize;
		memcpy(mappedPtr, data, minSize);
		vkUnmapMemory(mDevice, mDevMemory);
	}

	uint64_t VulkanBuffer::getBufferSize() const
	{
		return static_cast<uint64_t>(mBufferSize);
	}
	
	void* VulkanBuffer::mapBuffer(const uint64_t desiredSize, uint64_t& acutalSize)
	{
		desiredSize;
		acutalSize = 0;
		if (mMappedMemory != nullptr)
		{
			// error

		}
		else
		{
			// handle resize and copy data to larger requested size
			//createOrResizeBuffer(mBuffer, mDevMemory, mBufferSize, desiredSize);
			vkMapMemory(mDevice, mDevMemory, 0, mBufferSize, 0, &mMappedMemory);
			acutalSize = mUsedBufferSize;
		}

		return mMappedMemory;
	}

	void VulkanBuffer::unmapBuffer(void* ptr)
	{
		if (ptr == mMappedMemory)
		{
			if (ptr == nullptr)
			{
				// error
			}
			vkUnmapMemory(mDevice, mDevMemory);
			mMappedMemory = nullptr;
		}
		else
		{
			// error
		}
	}

	void VulkanBuffer::createOrResizeBuffer(VkBuffer& buffer, VkDeviceMemory& buffer_memory,
		VkDeviceSize& p_buffer_size, VkDeviceSize new_size)
	{

		VkResult err;
		if (buffer != VK_NULL_HANDLE)
			vkDestroyBuffer(mDevice, buffer, nullptr);
		if (buffer_memory)
			vkFreeMemory(mDevice, buffer_memory, nullptr);

		VkBufferUsageFlags usage = 0;
		switch (mBufferType)
		{
		case swizzle::eBufferType::eVertexBuffer:
			usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			break;
		default:
			break;
		}

		VkDeviceSize vertex_buffer_size_aligned =
			(((new_size - 1) / g_BufferMemoryAlignment) + 1) * g_BufferMemoryAlignment;
		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = vertex_buffer_size_aligned;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		err = vkCreateBuffer(mDevice, &buffer_info, nullptr, &buffer);
		// check_vk_result(err);

		VkMemoryRequirements req;
		vkGetBufferMemoryRequirements(mDevice, buffer, &req);
		g_BufferMemoryAlignment = (g_BufferMemoryAlignment > req.alignment) ? g_BufferMemoryAlignment : req.alignment;
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = req.size;
		alloc_info.memoryTypeIndex =
			mMemoryHelper.findMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
		err = vkAllocateMemory(mDevice, &alloc_info, nullptr, &buffer_memory);
		// check_vk_result(err);

		err = vkBindBufferMemory(mDevice, buffer, buffer_memory, 0);
		// check_vk_result(err);
		p_buffer_size = new_size;
	}

}