#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include <cstdint>

namespace swizzle
{

	enum class FramebufferAttachmentType
	{
		eDepth = 0,
		eDepthStencil,
		eColor
	};

	enum class eDepthType
	{
		eNone,
		eDepth,
		eDepthStencil,
	};

	struct ClearColor
	{
		float r, g, b, a;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() {}

		virtual const uint32_t getNumAttachments() const = 0;
		virtual const FramebufferAttachmentType getAttachmentType(uint32_t index) const = 0;
		virtual void setClearColor(uint32_t index, ClearColor color) = 0;
		virtual void setClearDepth(uint32_t index, float depthValue, uint8_t stencil) = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

	};

}

#endif