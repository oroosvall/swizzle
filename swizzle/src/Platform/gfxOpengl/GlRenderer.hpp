#ifndef GL_RENDERER_HPP
#define GL_RENDERER_HPP

#include <swizzle/Renderer.hpp>
#include <swizzle/Window.hpp>

namespace swizzle
{

	class GlRenderer : public Renderer, public swizzle::EventHandler<swizzle::WindowEvent>
	{
	public:

		GlRenderer(Window* window);

		virtual ~GlRenderer();

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


	private:

		bool mHadSubmission;

		// Inherited via EventHandler
		virtual void publishEvent(const swizzle::WindowEvent&) override;

	};

}

#endif