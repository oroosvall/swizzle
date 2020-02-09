#ifndef SWIZZLE_API_HPP
#define SWIZZLE_API_HPP

#if defined(SWIZZLE_DLL)
#if defined(SWIZZLE_DLL_EXPORT)
#define SWIZZLE_API __declspec(dllexport)
#else
#define SWIZZLE_API __declspec(dllimport)
#endif
#else
#define SWIZZLE_API
#endif

#endif