//#include "VulkanMesh.hpp"
//
//#include <memory>
//
//namespace renderer
//{
//
//	static VkDeviceSize g_BufferMemoryAlignment = 256;
//
//	VulkanMesh::VulkanMesh(VulkanMemoryHelper memoryHelper, VkDevice device, bool indexed)
//            : mMemoryHelper(memoryHelper), mDevice(device), mVertexBuffer(VK_NULL_HANDLE),
//              mVertexBufferSize(VK_NULL_HANDLE), mVertexMemory(VK_NULL_HANDLE), mIndexBuffer(VK_NULL_HANDLE),
//              mIndexBufferSize(VK_NULL_HANDLE), mIndexMemory(VK_NULL_HANDLE), mIndexed(indexed), mVertexMapped(false),
//              mIndexMapped(false)
//	{
//		
//	}
//
//	VulkanMesh::~VulkanMesh()
//	{
//		if (mVertexBuffer)
//		{
//			vkFreeMemory(mDevice, mVertexMemory, nullptr);
//			vkDestroyBuffer(mDevice, mVertexBuffer, nullptr);
//		}
//		if (mIndexed && mIndexBuffer)
//		{
//			vkFreeMemory(mDevice, mIndexMemory, nullptr);
//			vkDestroyBuffer(mDevice, mIndexBuffer, nullptr);
//		}
//	}
//	
//	eRenderApi VulkanMesh::getResourceApiType() const
//	{
//		return eRenderApi::eRenderer_Vulkan;
//	}
//
//	void VulkanMesh::setVertexData(const void* data, const size_t dataSize, eMeshData dataType)
//	{
//		// @TODO add staging buffers
//		if (dataSize > mVertexBufferSize)
//		{
//			createOrResizeBuffer(mVertexBuffer, mVertexMemory, mVertexBufferSize, dataSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
//		}
//		void* vtx_dst = nullptr;
//		vkMapMemory(mDevice, mVertexMemory, 0, mVertexBufferSize, 0, (void**)(&vtx_dst));
//
//		memcpy(vtx_dst, data, dataSize);
//
//		switch (dataType)
//		{
//		case renderer::eMeshData::eVertexData:
//			mNumVertices = dataSize / (sizeof(float) * 3);
//			break;
//		case renderer::eMeshData::eVertexUvData:
//			mNumVertices = dataSize / (sizeof(float) * 5);
//			break;
//		case renderer::eMeshData::eVertexUvColor:
//			mNumVertices = dataSize / (sizeof(float) * 9);
//			break;
//		case renderer::eMeshData::eVertexUvSkeletonData:
//			mNumVertices = dataSize / (sizeof(float) * 13);
//			break;
//		case renderer::eMeshData::eVertexUvColorSkeletonData:
//			mNumVertices = dataSize / (sizeof(float) * 17);
//			break;
//		default:
//			break;
//		}
//
//		VkMappedMemoryRange range[1] = {};
//		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
//		range[0].memory = mVertexMemory;
//		range[0].size = VK_WHOLE_SIZE;
//		vkFlushMappedMemoryRanges(mDevice, 1, range);
//		vkUnmapMemory(mDevice, mVertexMemory);
//	}
//
//	void VulkanMesh::setIndexData(const void* data, const size_t dataSize)
//	{
//		if (mIndexed)
//		{
//			// @TODO add staging buffers
//			if (dataSize > mIndexBufferSize)
//			{
//				createOrResizeBuffer(mIndexBuffer, mIndexMemory, mIndexBufferSize, dataSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
//			}
//
//			void* vtx_dst = nullptr;
//			vkMapMemory(mDevice, mIndexMemory, 0, mIndexBufferSize, 0, (void**)(&vtx_dst));
//
//			memcpy(vtx_dst, data, dataSize);
//
//			mNumVertices = dataSize / (sizeof(uint32_t) * 3);
//
//			VkMappedMemoryRange range[1] = {};
//			range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
//			range[0].memory = mIndexMemory;
//			range[0].size = VK_WHOLE_SIZE;
//			vkFlushMappedMemoryRanges(mDevice, 1, range);
//			vkUnmapMemory(mDevice, mIndexMemory);
//		}
//
//	}
//
//	void* VulkanMesh::mapVertexBuffer(const size_t bufferSize, eMeshData& dataType)
//	{
//		if (bufferSize > mVertexBufferSize)
//		{
//			createOrResizeBuffer(mVertexBuffer, mVertexMemory, mVertexBufferSize, bufferSize,
//				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
//		}
//
//		void* vtx_dst = nullptr;
//		vkMapMemory(mDevice, mVertexMemory, 0, mVertexBufferSize, 0, (void**)(&vtx_dst));
//		mVertexMapped = true;
//		return vtx_dst;
//	}
//
//	void* VulkanMesh::mapIndexBuffer(const size_t bufferSize)
//	{
//		void* idx_dst = nullptr;
//		if (mIndexed)
//		{
//			if (bufferSize > mIndexBufferSize)
//			{
//				createOrResizeBuffer(mIndexBuffer, mIndexMemory, mIndexBufferSize, bufferSize,
//					VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
//			}
//
//			vkMapMemory(mDevice, mIndexMemory, 0, mIndexBufferSize, 0, (void**)(&idx_dst));
//			mIndexMapped = true;
//		}
//		return idx_dst;
//	}
//
//	void VulkanMesh::flushAndUnmap()
//	{
//		if (mVertexMapped)
//		{
//			VkMappedMemoryRange range[1] = {};
//			range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
//			range[0].memory = mVertexMemory;
//			range[0].size = VK_WHOLE_SIZE;
//			vkFlushMappedMemoryRanges(mDevice, 1, range);
//			vkUnmapMemory(mDevice, mVertexMemory);
//		}
//		if (mIndexMapped)
//		{
//			VkMappedMemoryRange range[1] = {};
//			range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
//			range[0].memory = mIndexMemory;
//			range[0].size = VK_WHOLE_SIZE;
//
//			vkFlushMappedMemoryRanges(mDevice, 1, range);
//			vkUnmapMemory(mDevice, mIndexMemory);
//		}
//	}
//
//	void VulkanMesh::createOrResizeBuffer(VkBuffer& buffer, VkDeviceMemory& buffer_memory,
//		VkDeviceSize& p_buffer_size, size_t new_size,
//		VkBufferUsageFlagBits usage)
//	{
//		VkResult err;
//		if (buffer != VK_NULL_HANDLE)
//			vkDestroyBuffer(mDevice, buffer, nullptr);
//		if (buffer_memory)
//			vkFreeMemory(mDevice, buffer_memory, nullptr);
//
//		VkDeviceSize vertex_buffer_size_aligned =
//			(((new_size - 1) / g_BufferMemoryAlignment) + 1) * g_BufferMemoryAlignment;
//		VkBufferCreateInfo buffer_info = {};
//		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//		buffer_info.size = vertex_buffer_size_aligned;
//		buffer_info.usage = usage;
//		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//		err = vkCreateBuffer(mDevice, &buffer_info, nullptr, &buffer);
//		// check_vk_result(err);
//
//		VkMemoryRequirements req;
//		vkGetBufferMemoryRequirements(mDevice, buffer, &req);
//		g_BufferMemoryAlignment = (g_BufferMemoryAlignment > req.alignment) ? g_BufferMemoryAlignment : req.alignment;
//		VkMemoryAllocateInfo alloc_info = {};
//		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		alloc_info.allocationSize = req.size;
//		alloc_info.memoryTypeIndex =
//			mMemoryHelper.findMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
//		err = vkAllocateMemory(mDevice, &alloc_info, nullptr, &buffer_memory);
//		// check_vk_result(err);
//
//		err = vkBindBufferMemory(mDevice, buffer, buffer_memory, 0);
//		// check_vk_result(err);
//		p_buffer_size = new_size;
//	}
//
//}