#ifndef COMMAND_BUFFER_HPP
#define COMMAND_BUFFER_HPP

#include <cstdint>

#include <swizzle/Resource.hpp>

#include <swizzle/gfx/FrameBuffer.hpp>
#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/Shader.hpp>


namespace swizzle
{

	enum class eCmdBufferType
	{
		ePrimary,
		eSecondary,
	};

	class CommandBuffer
	{
	public:

		virtual ~CommandBuffer() {}

		virtual eCmdBufferType getBufferType() const = 0;

		virtual void resetBuffer(bool hardReset = false) = 0;

		virtual void bindFrameBuffer(Resource<FrameBuffer> framebuffer) = 0;
		virtual void unbindFrameBuffer() = 0;

		virtual void setViewport(uint32_t x, uint32_t y) = 0;

		virtual void bindShaderProgram(Resource<ShaderProgram> shaderProgram) = 0;
		virtual void setShaderConstantData(Resource<ShaderProgram> shaderProgram, void* data, uint32_t dataSize) = 0;
		virtual void draw(Resource<Buffer>& vertexBuffer) = 0;

		virtual void resolve(Resource<FrameBuffer> from, Resource<FrameBuffer> to) = 0;

		virtual void beginRecording() = 0;
		virtual void endRecording() = 0;


	};

}

#endif