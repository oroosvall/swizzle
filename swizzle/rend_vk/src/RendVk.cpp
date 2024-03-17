
/* Include files */

#include <rend_vk/RendVk.hpp>
#include "Log.hpp"

#include "Ctx.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace rvk
{
    const char* LogLevelToString(LogLevel lvl)
    {
        switch (lvl)
        {
        case rvk::LogLevel::Debug:
            return "Debug";
        case rvk::LogLevel::Info:
            return "Info";
        case rvk::LogLevel::Warning:
            return "Warning";
        case rvk::LogLevel::Error:
            return "Error";
        default:
            break;
        }
        return "Unknown";
    }

    bool VkRendInitialize(VkRendInitializeInfo& info)
    {
        SetLogFunc(info.mLoggingFunc);
        return true;
    }

    bool VkRendCleanup()
    {
        SetLogFunc(nullptr);
        return true;
    }

    std::shared_ptr<Context> VkRendCreateContext(bool debugCallbacks)
    {
        std::shared_ptr<Ctx> ctx = std::make_shared<Ctx>(debugCallbacks);
        if (ctx->isValid())
        {
            return ctx;
        }
        return nullptr;
    }

} // namespace rvk

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */