//#ifndef VULKAN_MESH_HPP
//#define VULKAN_MESH_HPP
//
//#include <renderer/IMesh.hpp>
//#include "VulkanMemoryHelper.hpp"
//
//#include <vulkan/vulkan.h>
//
//namespace renderer
//{
//
//	struct VulkanMeshResource
//	{
//		uint16_t mResourceType;
//		bool mHasIndex;
//		VkBuffer mVertexBuffer;
//		VkBuffer mIndexBuffer;
//		uint32_t vertexCount;
//	};
//
//	class VulkanMesh : public IMesh
//	{
//	public:
//
//		VulkanMesh(VulkanMemoryHelper memoryHelper, VkDevice device, bool indexed);
//		virtual ~VulkanMesh();
//
//		virtual eRenderApi getResourceApiType() const override;
//
//		virtual void setVertexData(const void* data, const size_t dataSize, eMeshData dataType) override;
//		virtual void setIndexData(const void* data, const size_t dataSize) override;
//
//		virtual void* mapVertexBuffer(const size_t bufferSize, eMeshData& dataType) override;
//		virtual void* mapIndexBuffer(const size_t bufferSize) override;
//
//		virtual void flushAndUnmap() override;
//
//	private:
//
//		void createOrResizeBuffer(VkBuffer& buffer, VkDeviceMemory& buffer_memory,
//			VkDeviceSize& p_buffer_size, size_t new_size,
//			VkBufferUsageFlagBits usage);
//
//		VulkanMemoryHelper mMemoryHelper;
//		VkDevice mDevice;
//
//		VkBuffer mVertexBuffer;
//		VkDeviceSize mVertexBufferSize;
//		VkDeviceMemory mVertexMemory;
//
//		VkBuffer mIndexBuffer;
//		VkDeviceSize mIndexBufferSize;
//		VkDeviceMemory mIndexMemory;
//
//		uint32_t mNumVertices;
//
//		bool mIndexed;
//
//		bool mVertexMapped;
//		bool mIndexMapped;
//
//	};
//}
//
//#endif