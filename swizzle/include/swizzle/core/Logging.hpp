#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <common/Common.hpp>
#include <swizzle/Api.hpp>

/*

All logger functions are allowed to be called before SwInitialize

*/

namespace swizzle::core
{
    class LogDevice
    {
    public:
        virtual ~LogDevice() {}
        virtual void logMessage(const SwChar* messageType, const SwChar* message) = 0;
    };

    void SWIZZLE_API LogMessage(const SwChar* messageType, const SwChar* message, ...);

    void SWIZZLE_API AddLogger(LogDevice* device);
    void SWIZZLE_API RemoveLogger(LogDevice* device);

} // namespace swizzle::core

#define LOG_INFO(msg, ...) swizzle::core::LogMessage("Info", msg, ##__VA_ARGS__);
#define LOG_WARNING(msg, ...) swizzle::core::LogMessage("Warning", msg, ##__VA_ARGS__);
#define LOG_ERROR(msg, ...) swizzle::core::LogMessage("Error", msg, ##__VA_ARGS__);
#define LOG_CRITICAL(msg, ...) swizzle::core::LogMessage("Critical", msg, ##__VA_ARGS__);

#endif