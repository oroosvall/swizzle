
#include "VkDebug.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include <memory>
#include <atomic>

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
        UNUSED_ARG(userData);
        UNUSED_ARG(msgCode);
        UNUSED_ARG(location);
        UNUSED_ARG(sourceObj);
        UNUSED_ARG(objType);

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
            std::cout << "ERR" << std::endl;
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

    std::atomic_uint64_t gAllocCount = 0ull;
    std::atomic_uint64_t gInternalAllocCount = 0ull;

    void* vkAllocFunc(void* pUser, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
    {
        UNUSED_ARG(pUser);
        UNUSED_ARG(allocationScope);
        void* ptr = _aligned_malloc(size, alignment);
        //printf("[VK_ALLOC_%s]: %p\n", allocScope(allocationScope), ptr);
        gAllocCount++;

        return ptr;
    }

    void vkFreeFunc(void* pUser, void* pMemory)
    {
        UNUSED_ARG(pUser);
        if (pMemory)
        {
            //printf("[VK_FREE]: %p\n", pMemory);
            _aligned_free(pMemory);
            gAllocCount--;
        }
    }

    void* vkReallocFunc(void* pUser, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
    {
        pUser; allocationScope;
        //printf("[VK_REALLOC_%s]: %p\n", allocScope(allocationScope), pOriginal);
        return _aligned_realloc(pOriginal, size, alignment);
    }

    void vkInternalAlloc(void* pUser, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
    {
        UNUSED_ARG(pUser);
        UNUSED_ARG(size);
        UNUSED_ARG(allocationType);
        UNUSED_ARG(allocationScope);
        //printf("Internal Alloc\n");
        gInternalAllocCount++;
    }

    void vkInternalFree(void* pUser, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
    {
        UNUSED_ARG(pUser);
        UNUSED_ARG(size);
        UNUSED_ARG(allocationType);
        UNUSED_ARG(allocationScope);
        printf("Internal Free\n");
        gInternalAllocCount--;
    }

#endif
    VkAllocationCallbacks* GetDebugAllocCallback(SwBool enable)
    {
        UNUSED_ARG(enable);
#if _DEBUG
        VkAllocationCallbacks* ret = nullptr;
        if (enable)
        {
            static VkAllocationCallbacks allocCallback = {};

            allocCallback.pUserData = VK_NULL_HANDLE;
            allocCallback.pfnAllocation = &vkAllocFunc;
            allocCallback.pfnFree = &vkFreeFunc;
            allocCallback.pfnReallocation = &vkReallocFunc;
            allocCallback.pfnInternalAllocation = &vkInternalAlloc;
            allocCallback.pfnInternalFree = &vkInternalFree;

            ret = &allocCallback;
        }
        return ret;
#else
        return VK_NULL_HANDLE;
#endif
    }

#if _DEBUG
    U64 getAllocCount()
    {
        return gAllocCount;
    }

    U64 getAllocInternalCount()
    {
        return gInternalAllocCount;
    }
#else
    U64 getAllocCount()
    {
        return 0ull;
    }

    U64 getAllocInternalCount()
    {
        return 0ull;
    }
#endif
}