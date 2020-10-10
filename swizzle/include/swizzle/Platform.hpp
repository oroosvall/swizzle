#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <swizzle/Api.hpp>
#include <cstdint>

namespace swizzle
{
	namespace platform
	{

		uint64_t SWIZZLE_API getPlatformTimeStamp();
		const uint32_t SWIZZLE_API getPlatformCurrentThreadId();

	}
}

#endif