#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstdint>

namespace swizzle
{

	enum class eBufferType
	{
		eVertexBuffer,
	};

	class Buffer
	{
	public:
		virtual ~Buffer() {}

		virtual eBufferType getBufferType() const = 0;

		virtual void setBufferData(void* data, const uint64_t size) = 0;
		virtual uint64_t getBufferSize() const = 0;

		virtual void* mapBuffer(const uint64_t desiredSize, uint64_t& acutalSize) = 0;
		virtual void unmapBuffer(void* ptr) = 0;

	};

}

#endif