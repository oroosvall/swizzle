
/* Include files */

#include <rend_vk/RendVk.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

namespace rvk
{
    LogFunc* gLoggingFunc = nullptr;
}

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace rvk
{
    void SetLogFunc(LogFunc* fn)
    {
        gLoggingFunc = fn;
    }

    void LogMessage(LogLevel level, const char* message, ...)
    {
        if (gLoggingFunc)
        {
            va_list vl;
            va_start(vl, message);
            gLoggingFunc(level, message, vl);

            va_end(vl);
        }
    }

} // namespace rvk

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
