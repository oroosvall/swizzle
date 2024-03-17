
#pragma once

/* Include files */

#include <rend_vk/Context.hpp>

#include <cstdarg>
#include <memory>

/* Defines */

#define RVK_UNUSED_ARG(x) (void)x;

/* Typedefs/enums */

namespace rvk
{
    enum class LogLevel
    {
        Debug,
        Info,
        Warning,
        Error
    };

    using LogFunc = void(LogLevel, const char*, va_list);
} // namespace rvk

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace rvk
{
    struct VkRendInitializeInfo
    {
        LogFunc* mLoggingFunc;
    };
} // namespace rvk

/* Class Declaration */

/* Function Declaration */

namespace rvk
{
    /// <summary>
    /// Helper function to convert logleve to string
    /// </summary>
    /// <param name="lvl">log level to convert</param>
    /// <returns>string representation of log level</returns>
    const char* LogLevelToString(LogLevel lvl);

    /// <summary>
    /// Initialize vulkan render library,
    /// Must be called before creating a context.
    /// </summary>
    /// <param name="info">intiailization parameters</param>
    /// <returns>true on success, false on failure</returns>
    bool VkRendInitialize(VkRendInitializeInfo& info);

    /// <summary>
    /// Cleanup vulkan render library.
    /// Must be called after cleanup of all contexts
    /// </summary>
    /// <returns>true on success, false on failure</returns>
    bool VkRendCleanup();

    /// <summary>
    /// Create a rendering context
    /// </summary>
    /// <param name="debugCallbacks">bool if debug callbacks should be initialized</param>
    /// <returns>shared ptr to context, nullptr on failure</returns>
    std::shared_ptr<Context> VkRendCreateContext(bool debugCallbacks);
} // namespace rvk
