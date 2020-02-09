
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

}