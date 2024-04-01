
/* Include files */

#include <common/Common.hpp>

#include "../Log.hpp"

#include "Debug.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include <memory>
#include <atomic>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

static bool replace(std::string& str, const std::string& from, const std::string& to);
static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
                                                      uint64_t sourceObj, size_t location, int32_t msgCode,
                                                      const char* layerPrefix, const char* msg, void* userData);

/* Static Function Definition */

static bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
                                                      uint64_t sourceObj, size_t location, int32_t msgCode,
                                                      const char* layerPrefix, const char* msg, void* userData)
{
    UNUSED_ARG(userData);
    UNUSED_ARG(msgCode);
    UNUSED_ARG(location);
    UNUSED_ARG(sourceObj);
    UNUSED_ARG(objType);

    std::ostringstream stream;

    stream << "@[" << layerPrefix << "]:\n";
    std::string msgStr(msg);

    replace(msgStr, ". ", ".\n");

    if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        stream << msgStr << std::endl;
        //LOG_INFO("%s", stream.str().c_str());
    }
    else if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        stream << msgStr << std::endl;
        rvk::LogMessage(rvk::LogLevel::Warning, "%s", stream.str().c_str());
    }
    else if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        stream << "PERF: ";
        stream << msgStr << std::endl;
        rvk::LogMessage(rvk::LogLevel::Info, "%s", stream.str().c_str());
    }
    else if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        stream << "DEBUG: ";
        stream << msgStr << std::endl;
        //LOG_INFO("%s", stream.str().c_str());
    }
    else if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        stream << msgStr << std::endl;
        rvk::LogMessage(rvk::LogLevel::Error, "%s", stream.str().c_str());
    }

    return VK_FALSE;
}

/* Function Definition */

namespace rvk
{
    VkDebugReportCallbackCreateInfoEXT getDebugCallbacks()
    {
        VkDebugReportCallbackCreateInfoEXT debugCallbackInfo{};
        debugCallbackInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debugCallbackInfo.flags = VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT |
            //VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
            VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT;
        debugCallbackInfo.pNext = VK_NULL_HANDLE;
        debugCallbackInfo.pUserData = VK_NULL_HANDLE;
        debugCallbackInfo.pfnCallback = &vkDebugCallback;

        return debugCallbackInfo;
    }

} // namespace rvk

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
