#include "GlRenderer.hpp"

namespace swizzle
{



	GlRenderer::GlRenderer(Window* window)
	{
		window;
	}

	GlRenderer::~GlRenderer()
	{
	}

	Resource<Buffer> GlRenderer::createBuffer(eBufferType)
	{
		return Resource<Buffer>();
	}

	Resource<CommandBuffer> GlRenderer::createCommandBuffer(eCmdBufferType)
	{
		return Resource<CommandBuffer>();
	}

	Resource<FrameBuffer> GlRenderer::createFrameBuffer(uint32_t numAttach, DepthType depth, uint32_t width, uint32_t height)
	{
		numAttach;
		depth;
		width;
		height;
		return Resource<FrameBuffer>();
	}

	void GlRenderer::submit(Resource<CommandBuffer> cmdbuffer)
	{
	}

	Resource<FrameBuffer> GlRenderer::getDefaultFramebuffer()
	{
		return Resource<FrameBuffer>();
	}

	Resource<Texture2D> GlRenderer::createTexture(eTextureDataFormat type)
	{
		type;
		return Resource<Texture2D>();
	}

	Resource<ShaderProgram> GlRenderer::createShaderProgram(const Resource<FrameBuffer>& fbo, eShaderProgramType programType)
	{
		fbo;
		programType;
		return Resource<ShaderProgram>();
	}

	void GlRenderer::prepare(Resource<CommandBuffer> cmdbuf)
	{
		cmdbuf;
	}

	void GlRenderer::present()
	{
	}

	bool GlRenderer::isVsyncModeSupported(VSyncTypes sync) const
	{
		sync;
		return false;
	}

	void GlRenderer::setVsync(VSyncTypes sync)
	{
		sync;
	}

	void GlRenderer::publishEvent(const swizzle::WindowEvent&)
	{
	}

}