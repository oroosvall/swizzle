#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <common/Types.hpp>
#include <swizzle/Api.hpp>

/*

All logger functions are allowed to be called before SwInitialize

*/

namespace swizzle::core
{
    class LogDevice
    {
    public:
        virtual void logMessage(SwChar* messageType, SwChar* message) = 0;
    };


    void SWIZZLE_API LogMessage(SwChar* messageType, SwChar* message, ...);

    void SWIZZLE_API AddLogger(LogDevice* device);
    void SWIZZLE_API RemoveLogger(LogDevice* device);

}


#define LOG_INFO(msg, ...) swizzle::core::LogMessage("Info", msg, __VA_ARGS__);
#define LOG_WARNING(msg, ...) swizzle::core::LogMessage("Warning", msg, __VA_ARGS__);
#define LOG_ERROR(msg, ...) swizzle::core::LogMessage("Error", msg, __VA_ARGS__);
#define LOG_CRITICAL(msg, ...) swizzle::core::LogMessage("Critical", msg, __VA_ARGS__);

#endif