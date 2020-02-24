
#include "VulkanCommandBuffer.hpp"

#include "VulkanFrameBuffer.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanShader.hpp"

namespace swizzle
{

	VulkanCommandBuffer::VulkanCommandBuffer(VkDevice device, VkCommandPool cmdPool, eCmdBufferType cmdType)
		: mDevice(device)
		, mCommandPool(cmdPool)
		, mCommandBuffer(VK_NULL_HANDLE)
		, mCommandBufferType(cmdType)
		, mLastBoundFrameBuffer(nullptr)
		, mLastBoundShaderProgram(nullptr)
		, mRecording(false)
	{

		VkCommandBufferAllocateInfo allocInfo;

		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.pNext = VK_NULL_HANDLE;
		allocInfo.commandPool = mCommandPool;
		if (mCommandBufferType == eCmdBufferType::ePrimary)
		{
			allocInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		}
		else
		{
			allocInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		}
		allocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(mDevice, &allocInfo, &mCommandBuffer);
		
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		vkFreeCommandBuffers(mDevice, mCommandPool, 1, &mCommandBuffer);
	}

	eCmdBufferType VulkanCommandBuffer::getBufferType() const
	{
		return mCommandBufferType;
	}

	void VulkanCommandBuffer::resetBuffer(bool hardReset)
	{
		if(hardReset)
		{
			vkResetCommandBuffer(mCommandBuffer, VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		}
		else
		{
			vkResetCommandBuffer(mCommandBuffer, 0);
		}
	}

	void VulkanCommandBuffer::beginRecording()
	{
		if (!mRecording)
		{
			VkCommandBufferBeginInfo beginInfo;

			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = VK_NULL_HANDLE;
			beginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = VK_NULL_HANDLE;

			if (mCommandBufferType == eCmdBufferType::eSecondary)
			{
				VkCommandBufferInheritanceInfo inhertianceInfo;

				inhertianceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
				inhertianceInfo.pNext = VK_NULL_HANDLE;
				inhertianceInfo.renderPass = VK_NULL_HANDLE;
				inhertianceInfo.subpass = 0;
				inhertianceInfo.framebuffer = VK_NULL_HANDLE;
				inhertianceInfo.occlusionQueryEnable = VK_FALSE;
				inhertianceInfo.queryFlags = VkQueryControlFlagBits::VK_QUERY_CONTROL_PRECISE_BIT;
				inhertianceInfo.pipelineStatistics = VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT;

				beginInfo.pInheritanceInfo = &inhertianceInfo;
			}

			vkBeginCommandBuffer(mCommandBuffer, &beginInfo);
			mRecording = true;
		}
	}

	void VulkanCommandBuffer::endRecording()
	{
		mLastBoundShaderProgram.reset();
		unbindFrameBuffer();
		vkEndCommandBuffer(mCommandBuffer);
		mRecording = false;
	}

	void VulkanCommandBuffer::setViewport(uint32_t x, uint32_t y)
	{
		VkViewport vp;
		vp.x = 0;
		vp.y = static_cast<float_t>(y);
		vp.width = static_cast<float_t>(x);
		vp.height = -static_cast<float_t>(y);
		vp.minDepth = 0.0F;
		vp.maxDepth = 1.0F;
		vkCmdSetViewport(mCommandBuffer, 0, 1, &vp);
		VkRect2D r;
		r.extent.width = x;
		r.extent.height = y;
		r.offset.x = 0;
		r.offset.y = 0;
		vkCmdSetScissor(mCommandBuffer, 0, 1, &r);
	}
	
	void VulkanCommandBuffer::bindShaderProgram(Resource<ShaderProgram> shaderProgram)
	{
		if (mLastBoundShaderProgram)
		{
			// not sure what do do here yet
		}
		mLastBoundShaderProgram = shaderProgram;
		
		ShaderProgram* prog = shaderProgram.get();
		VulkanShaderProgram* vkProg = (VulkanShaderProgram*)prog;
		
		vkCmdBindPipeline(mCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, vkProg->mPipeline);
	}

	void VulkanCommandBuffer::setShaderConstantData(Resource<ShaderProgram> shaderProgram, void* data, uint32_t dataSize)
	{
		ShaderProgram* prog = shaderProgram.get();
		VulkanShaderProgram* vkProg = (VulkanShaderProgram*)prog;

		vkCmdPushConstants(mCommandBuffer, vkProg->mPipelineLayout, VK_SHADER_STAGE_ALL, 0, dataSize, data);
	}

	void VulkanCommandBuffer::draw(Resource<Buffer>& vertexBuffer)
	{
		if (mLastBoundShaderProgram)
		{
			Buffer* vbuf = vertexBuffer.get();
			VulkanBuffer* vkBuffer = (VulkanBuffer*)vbuf;
			if (vkBuffer->getBufferType() == swizzle::eBufferType::eVertexBuffer)
			{
				VkDeviceSize size = 0;
				vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, &vkBuffer->mBuffer, &size);
				uint32_t numVertices = static_cast<uint32_t>(vkBuffer->getBufferSize() / (sizeof(float) * 3));
				vkCmdDraw(mCommandBuffer, numVertices, 1, 0, 0);
			}
		}

	}

	//void VulkanCommandBuffer::submit(engine::Ref<IMaterial> material, engine::Ref<IMesh> mesh)
	//{

	//}

	void VulkanCommandBuffer::bindFrameBuffer(Resource<FrameBuffer> framebuffer)
	{
		if (mRecording)
		{
			//if (framebuffer->getResourceApiType() != eRenderApi::eRenderer_Vulkan)
			//{
			//	// log error with mixing api
			//	return;
			//}

			if (mLastBoundFrameBuffer)
			{
				unbindFrameBuffer();
			}
			mLastBoundFrameBuffer = framebuffer;

			FrameBuffer* ifbo = framebuffer.get();
			BaseFrameBuffer* fbo = reinterpret_cast<BaseFrameBuffer*>(ifbo);

			VkRenderPassBeginInfo beginInfo;
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.pNext = VK_NULL_HANDLE;
			beginInfo.renderPass = fbo->getRenderPass();
			beginInfo.framebuffer = fbo->getFramebuffer();
			beginInfo.renderArea = fbo->getRenderArea();
			beginInfo.clearValueCount = fbo->getImageCount();
			beginInfo.pClearValues = fbo->getImageClearValues();

			VkSubpassContents subpass = VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE;

			vkCmdBeginRenderPass(mCommandBuffer, &beginInfo, subpass);
			

			//vkCmdBindPipeline(mCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, )

		}
	}

	void VulkanCommandBuffer::unbindFrameBuffer()
	{
		if (mRecording)
		{
			if (mLastBoundFrameBuffer)
			{
				vkCmdEndRenderPass(mCommandBuffer);
				mLastBoundFrameBuffer.reset();
			}
		}
	}

	void VulkanCommandBuffer::resolve(Resource<FrameBuffer> from, Resource<FrameBuffer> to)
	{
		if (mRecording)
		{
			FrameBuffer* ifbo = from.get();
			BaseFrameBuffer* fbo = reinterpret_cast<BaseFrameBuffer*>(ifbo);

			FrameBuffer* idfbo = to.get();
			BaseFrameBuffer* dfbo = reinterpret_cast<BaseFrameBuffer*>(idfbo);

			VkImage srcImage = fbo->getImage(1);
			VkImage dstImage = dfbo->getImage(0);

			VkImageResolve resolve;
			resolve.srcOffset = { 0, 0, 0 };
			resolve.dstOffset = { 0, 0, 0 };
			auto [x, y] = dfbo->getRenderArea().extent;
			resolve.extent = { x, y, 0U };
			resolve.srcSubresource.mipLevel = 0U;
			resolve.srcSubresource.layerCount = 1U;
			resolve.srcSubresource.baseArrayLayer = 0U;
			resolve.srcSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
				
			resolve.dstSubresource.mipLevel = 0U;
			resolve.dstSubresource.layerCount = 1U;
			resolve.dstSubresource.baseArrayLayer = 0U;
			resolve.dstSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

			VkImageMemoryBarrier imageBarrier[2];

			imageBarrier[0].sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier[0].pNext =  VK_NULL_HANDLE;
			imageBarrier[0].srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			imageBarrier[0].dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			imageBarrier[0].oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			imageBarrier[0].newLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			imageBarrier[0].srcQueueFamilyIndex = 0;
			imageBarrier[0].dstQueueFamilyIndex = 0;
			imageBarrier[0].image = srcImage;
			imageBarrier[0].subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageBarrier[0].subresourceRange.baseArrayLayer = 0U;
			imageBarrier[0].subresourceRange.baseMipLevel = 0U;
			imageBarrier[0].subresourceRange.layerCount = 1U;
			imageBarrier[0].subresourceRange.levelCount = 1U;

			imageBarrier[1].sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier[1].pNext = VK_NULL_HANDLE;
			imageBarrier[1].srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			imageBarrier[1].dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			imageBarrier[1].oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			imageBarrier[1].newLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier[1].srcQueueFamilyIndex = 0;
			imageBarrier[1].dstQueueFamilyIndex = 0;
			imageBarrier[1].image = dstImage;
			imageBarrier[1].subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageBarrier[1].subresourceRange.baseArrayLayer = 0U;
			imageBarrier[1].subresourceRange.baseMipLevel = 0U;
			imageBarrier[1].subresourceRange.layerCount = 1U;
			imageBarrier[1].subresourceRange.levelCount = 1U;

			vkCmdPipelineBarrier(mCommandBuffer
				, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
				, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
				, VkDependencyFlagBits::VK_DEPENDENCY_DEVICE_GROUP_BIT
				, 0, VK_NULL_HANDLE
				, 0, VK_NULL_HANDLE
				, 2, imageBarrier);

			if (fbo->getMultisampleCount() != VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT)
			{
				vkCmdResolveImage(mCommandBuffer, srcImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &resolve);
			}
			else
			{
				VkImageBlit blt;
				int32_t xx = static_cast<int32_t>(x);
				int32_t yy = static_cast<int32_t>(y);

				blt.dstOffsets[0] = { 0, 0, 0 };
				blt.dstOffsets[1] = { xx, yy, 1 };
				blt.srcOffsets[0] = { 0, 0, 0 };
				blt.srcOffsets[1] = { xx, yy, 1 };

				blt.dstSubresource = resolve.dstSubresource;
				blt.srcSubresource = resolve.srcSubresource;

				vkCmdBlitImage(mCommandBuffer, srcImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blt, VkFilter::VK_FILTER_NEAREST);
			}

			imageBarrier[0].newLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			imageBarrier[0].oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

			imageBarrier[1].oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier[1].newLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			vkCmdPipelineBarrier(mCommandBuffer
				, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
				, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
				, VkDependencyFlagBits::VK_DEPENDENCY_DEVICE_GROUP_BIT
				, 0, VK_NULL_HANDLE
				, 0, VK_NULL_HANDLE
				, 2, imageBarrier);

		}
	}

}