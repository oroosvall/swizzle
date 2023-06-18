
#pragma once

/* Includes */

#if defined(OTP_PROFILING) && defined(OPTICK_ENABLED)
#include <optick/optick.h>
#endif

/* Defines */

#if defined(OTP_PROFILING) && defined(OPTICK_ENABLED)

#define SWIZZLE_PROFILE_FRAME(frameName, ...) OPTICK_FRAME(frameName, __VA_ARGS__)
#define SWIZZLE_PROFILE_EVENT(...) OPTICK_EVENT(__VA_ARGS__)
#define SWIZZLE_PROFILE_THREAD(...) OPTICK_THREAD(...)

#else
#define SWIZZLE_PROFILE_FRAME(frameName, ...)
#define SWIZZLE_PROFILE_EVENT(...)
#define SWIZZLE_PROFILE_THREAD(...)
#endif

/* Typedefs */

/* Function Declaration */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

