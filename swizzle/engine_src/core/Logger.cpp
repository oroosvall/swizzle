#include <swizzle/core/Logging.hpp>

#include <cstdarg>
#include <string>
#include <list>
#include <stdio.h>
#include <stdarg.h>

namespace swizzle::core
{
    std::list<LogDevice*> gLoggers;

    void LogMessage(const SwChar* messageType, const SwChar* message, ...)
    {
        va_list vl;
        va_start(vl, message);

        SwChar buffer[1024];

        vsnprintf(buffer, sizeof(buffer), message, vl);

        for (auto it : gLoggers)
        {
            it->logMessage(messageType, buffer);
        }

        va_end(vl);
    }

    void AddLogger(LogDevice* device)
    {
        gLoggers.emplace_back(device);
    }

    void RemoveLogger(LogDevice* device)
    {
        gLoggers.remove(device);
    }

}