#ifndef VULKAN_COMMAND_BUFFER_HPP
#define VULKAN_COMMAND_BUFFER_HPP

#include "vk.hpp"

#include <swizzle/Resource.hpp>
#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/Shader.hpp>

namespace swizzle
{

	class VulkanCommandBuffer : public CommandBuffer
	{
	public:

		VulkanCommandBuffer(VkDevice device, VkCommandPool cmdPool, eCmdBufferType cmdType);
		virtual ~VulkanCommandBuffer();

		virtual eCmdBufferType getBufferType() const override;

		virtual void resetBuffer(bool hardReset) override;

		virtual void beginRecording() override;
		virtual void endRecording() override;

		virtual void setViewport(uint32_t x, uint32_t y) override;

		virtual void bindShaderProgram(Resource<ShaderProgram> shaderProgram) override;
		virtual void setShaderConstantData(Resource<ShaderProgram> shaderProgram, void* data, uint32_t dataSize) override;
		virtual void draw(Resource<Buffer>& vertexBuffer) override;

		virtual void VulkanCommandBuffer::bindFrameBuffer(Resource<FrameBuffer> framebuffer) override;
		virtual void VulkanCommandBuffer::unbindFrameBuffer() override;

		VkCommandBuffer getCommandBuffer() { return mCommandBuffer; };
		bool isRecording() const { return mRecording; };

	private:

		VkDevice mDevice;
		VkCommandPool mCommandPool;
		
		VkCommandBuffer mCommandBuffer;

		eCmdBufferType mCommandBufferType;

		Resource<FrameBuffer> mLastBoundFrameBuffer;
		Resource<ShaderProgram> mLastBoundShaderProgram;

		bool mRecording;

	};

}

#endif