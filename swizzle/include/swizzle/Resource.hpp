#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <memory>

namespace swizzle
{
	template<typename T>
	using Resource = std::shared_ptr<T>;

}

#endif