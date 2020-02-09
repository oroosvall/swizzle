#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include <swizzle/Renderer.hpp>
#include <swizzle/Window.hpp>

#include "vk.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInstance.hpp"
#include "VulkanMemoryHelper.hpp"

namespace swizzle
{

	class VulkanRenderer : public Renderer, public swizzle::EventHandler<swizzle::WindowEvent>
	{
	public:

		VulkanRenderer(Window* window);

		virtual ~VulkanRenderer();

		virtual Resource<Buffer> createBuffer(eBufferType) override;
		virtual Resource<CommandBuffer> createCommandBuffer(eCmdBufferType) override;
		virtual Resource<FrameBuffer> createFrameBuffer(uint32_t numAttach, DepthType depth, uint32_t width, uint32_t height) override;

		virtual void submit(Resource<CommandBuffer> cmdbuffer) override;

		virtual Resource<FrameBuffer> getDefaultFramebuffer() override;

		virtual Resource<Texture2D> createTexture(eTextureDataFormat type) override;
		virtual Resource<ShaderProgram> createShaderProgram(const Resource<FrameBuffer>& fbo, eShaderProgramType programType) override;

		virtual void prepare(Resource<CommandBuffer> cmdbuf) override;
		virtual void present() override;

		virtual bool isVsyncModeSupported(VSyncTypes sync) const override;
		virtual void setVsync(VSyncTypes sync) override;


		VkInstance getInstance() const;
		
	private:

		VulkanInstance* mInstance;
		PhysicalDevice* mPhysical;
		LogicalDevice* mLogical;
		VulkanSwapchain* mSwapchain;
		
		VulkanMemoryHelper* mMemoryHelper;

		bool mHadSubmission;

		VkPipelineCache mPipelineCache;


		// Inherited via EventHandler
		virtual void publishEvent(const swizzle::WindowEvent&) override;

	};

}

#endif