#ifndef VECTORS_HPP
#define VECTORS_HPP

#include  <swizzle/Api.hpp>

namespace swizzle
{

	struct SWIZZLE_API Vector2F
	{
		Vector2F() : x(0.0F), y(0.0F) {}
		Vector2F(float _x, float _y) : x(_x), y(_y) {}
		float x, y;
	};

	struct SWIZZLE_API Vector3F
	{
		Vector3F() : x(0.0F), y(0.0F), z(0.0F) {}
		Vector3F(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		float x, y, z;
	};

	struct SWIZZLE_API Vector4F
	{
		Vector4F() : x(0.0F), y(0.0F), z(0.0F), w(0.0F) {}
		Vector4F(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		float x, y, z, w;
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