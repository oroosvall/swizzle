
#include "VkDebug.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

#include <memory>

namespace vk
{
    bool replace(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }


    VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
        uint64_t sourceObj, size_t location, int32_t msgCode,
        const char* layerPrefix, const char* msg, void* userData)
    {

        userData;
        msgCode;
        location;
        sourceObj;
        objType;

        std::ostringstream stream;

        if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
        {
            stream << "INFO: ";
            //return VK_FALSE;
        }
        else if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            stream << "WARNING: ";
            //return VK_FALSE;
        }
        else if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        {
            stream << "PERF: ";
            //return VK_FALSE;
        }
        else if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT)
        {
            stream << "DEBUG: ";
            //return VK_FALSE;
        }
        else if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            stream << "ERROR: ";
        }
        stream << "@[" << layerPrefix << "]:\n";
        std::string msgStr(msg);

        replace(msgStr, ". ", ".\n");

        //msgStr.replace(msgStr.begin(), ". ", ".\n");

        stream << msgStr << std::endl << std::endl;

        if (strcmp(layerPrefix, "MEM") != 0)
        {
            std::cout << stream.str();
        }

        if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            int err = 0;
            err;
        }

        return VK_FALSE;
    }

#if _DEBUG

    const char* allocScope(VkSystemAllocationScope allocScope)
    {
        const char* str;

        switch (allocScope)
        {
        case VK_SYSTEM_ALLOCATION_SCOPE_COMMAND:
            str = "CMD";
            break;
        case VK_SYSTEM_ALLOCATION_SCOPE_OBJECT:
            str = "OBJ";
            break;
        case VK_SYSTEM_ALLOCATION_SCOPE_CACHE:
            str = "CACHE";
            break;
        case VK_SYSTEM_ALLOCATION_SCOPE_DEVICE:
            str = "DEV";
            break;
        case VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE:
            str = "INST";
            break;
        case VK_SYSTEM_ALLOCATION_SCOPE_MAX_ENUM:
        default:
            str = "UNDEF";
            break;
        }

        return str;
    }

    void* vkAllocFunc(void* pUser, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
    {
        pUser; allocationScope;
        void* ptr = _aligned_malloc(size, alignment);
        printf("[VK_ALLOC_%s]: %p\n", allocScope(allocationScope), ptr);
        return ptr;
    }

    void vkFreeFunc(void* pUser, void* pMemory)
    {
        if (pMemory)
        {
            pUser;
            printf("[VK_FREE]: %p\n", pMemory);
            _aligned_free(pMemory);
        }
    }

    void* vkReallocFunc(void* pUser, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
    {
        pUser; allocationScope;
        printf("[VK_REALLOC_%s]: %p\n", allocScope(allocationScope), pOriginal);
        return _aligned_realloc(pOriginal, size, alignment);
    }

    void vkInternalAlloc(void* pUser, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
    {
        pUser; size; allocationType; allocationScope;
        printf("Internal Alloc\n");
    }

    void vkInternalFree(void* pUser, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
    {
        pUser; size; allocationType; allocationScope;
        printf("Internal Free\n");
    }

#endif
    VkAllocationCallbacks* getDebugAllocCallback()
    {
//#if _DEBUG
//        static VkAllocationCallbacks allocCallback = {};
//
//        allocCallback.pUserData = VK_NULL_HANDLE;
//        allocCallback.pfnAllocation = &vkAllocFunc;
//        allocCallback.pfnFree = &vkFreeFunc;
//        allocCallback.pfnReallocation = &vkReallocFunc;
//        allocCallback.pfnInternalAllocation = &vkInternalAlloc;
//        allocCallback.pfnInternalFree = &vkInternalFree;
//
//        return &allocCallback;
//#else
        return VK_NULL_HANDLE;
//#endif
    }
}