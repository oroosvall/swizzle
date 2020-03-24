#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <swizzle/Resource.hpp>

#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/FrameBuffer.hpp>
#include <swizzle/gfx/Material.hpp>
#include <swizzle/gfx/Shader.hpp>
#include <swizzle/gfx/Texture.hpp>
#include <swizzle/gfx/Buffer.hpp>

namespace swizzle
{

	enum class eRendererBackend
	{
		eRendererVulkan,
		eRendererOpengl
	};

	enum class VSyncTypes
	{
		eSync_off,
		eSync_on,
		eSync_adaptive,
	};

	class Renderer
	{
	public:
		
		virtual ~Renderer() {}

		virtual bool isVsyncModeSupported(VSyncTypes sync) const = 0;
		virtual void setVsync(VSyncTypes sync) = 0;
		
		virtual Resource<Buffer> createBuffer(eBufferType) = 0;

		virtual Resource<CommandBuffer> createCommandBuffer(eCmdBufferType) = 0;
		virtual Resource<FrameBuffer> createFrameBuffer(uint32_t numAttach, eDepthType depth, uint32_t width, uint32_t height) = 0;

		virtual Resource<FrameBuffer> getDefaultFramebuffer() = 0;

		virtual Resource<Texture2D> createTexture(eTextureDataFormat type) = 0;
		virtual Resource<ShaderProgram> createShaderProgram(const Resource<FrameBuffer>& fbo, eShaderProgramType programType) = 0;



		virtual void submit(Resource<CommandBuffer> cmdbuffer) = 0;
		
		virtual void prepare(Resource<CommandBuffer> cmdbuf) = 0;
		virtual void present() = 0;

	};
}

#endif