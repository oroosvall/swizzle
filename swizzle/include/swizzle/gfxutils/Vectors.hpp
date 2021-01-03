#ifndef VECTORS_HPP
#define VECTORS_HPP

#include <swizzle/core/Types.hpp>
#include <swizzle/Api.hpp>

namespace swizzle
{

	struct SWIZZLE_API Vector2F
	{
		Vector2F() : x(0.0F), y(0.0F) {}
		Vector2F(F32 _x, F32 _y) : x(_x), y(_y) {}
		F32 x, y;
	};

	struct SWIZZLE_API Vector3F
	{
		Vector3F() : x(0.0F), y(0.0F), z(0.0F) {}
		Vector3F(F32 _x, F32 _y, F32 _z) : x(_x), y(_y), z(_z) {}
		F32 x, y, z;
	};

	struct SWIZZLE_API Vector4F
	{
		Vector4F() : x(0.0F), y(0.0F), z(0.0F), w(0.0F) {}
		Vector4F(F32 _x, F32 _y, F32 _z, F32 _w) : x(_x), y(_y), z(_z), w(_w) {}
		F32 x, y, z, w;
	};

	template<typename T, typename F>
	inline T convert2F(F& vec)
	{
		return T(vec.x, vec.y);
	}

	template<typename T, typename F>
	inline T convert3F(F& vec)
	{
		return T(vec.x, vec.y, vec.z);
	}

	template<typename T, typename F>
	inline T convert4F(F& vec)
	{
		return T(vec.x, vec.y, vec.z, vec.w);
	}

}

#endif