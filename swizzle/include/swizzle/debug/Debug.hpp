#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <common/Types.hpp>
#include <swizzle/Api.hpp>
#include <swizzle/core/Logging.hpp>

#ifdef SW_DEBUG
#define DEBUG_LOG(msg, ...) swizzle::core::LogMessage("DEBUG", msg, __VA_ARGS__);
#else 
#define DEBUG_LOG(msg, ...)
#endif

#endif