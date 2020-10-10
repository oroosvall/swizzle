//#ifndef VULKAN_STAGING_BUFFER_HPP
//#define VULKAN_STAGING_BUFFER_HPP
//
//#include <vulkan/vulkan.h>
//
//#include "VulkanCommandBuffer.hpp"
//
//namespace renderer
//{
//
//	class StagingBuffer
//	{
//	public:
//
//		static StagingBuffer* getStagingBuffer();
//		static void release();
//
//
//		void beginStaging();
//		void endStaging();
//
//		void setCommandBuffer(engine::Ref<VulkanCommandBuffer> cmdBuffer);
//		engine::Ref<VulkanCommandBuffer> getCommandBuffer() const;
//
//
//	private:
//
//		static StagingBuffer* sInstange;
//
//		engine::Ref<VulkanCommandBuffer> mCommandBuffer;
//
//
//
//	};
//
//}
//
//#endif