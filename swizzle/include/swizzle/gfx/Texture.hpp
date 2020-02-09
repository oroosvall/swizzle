#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <cstdint>

namespace swizzle
{

	enum class eTextureDataFormat
	{
		RInt8,
		RGInt8,
		RGBInt8,
		RGBAInt8,
		RFloat,
		RGFloat,
		RGBFloat,
		RGBAFloat
	};

	class Texture2D
	{
	public:

		virtual ~Texture2D() = default;

		virtual const uint32_t getWidth() const = 0;
		virtual const uint32_t getHeight() const = 0;

		//
		virtual void setTextureData(uint32_t width, uint32_t height, void* data) = 0;

	};

}

#endif