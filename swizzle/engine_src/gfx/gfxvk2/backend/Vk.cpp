#include "Vk.hpp"

#include <map>

#include <swizzle/core/Logging.hpp>

#define RES_STR(a) {a, #a}

namespace vk
{
    static std::map<VkResult, const char*> vkResultStrings =
    {
        RES_STR(VK_SUCCESS),
        RES_STR(VK_NOT_READY),
        RES_STR(VK_TIMEOUT),
        RES_STR(VK_EVENT_SET),
        RES_STR(VK_EVENT_RESET),
        RES_STR(VK_INCOMPLETE),
        RES_STR(VK_ERROR_OUT_OF_HOST_MEMORY),
        RES_STR(VK_ERROR_OUT_OF_DEVICE_MEMORY),
        RES_STR(VK_ERROR_INITIALIZATION_FAILED),
        RES_STR(VK_ERROR_DEVICE_LOST),
        RES_STR(VK_ERROR_MEMORY_MAP_FAILED),
        RES_STR(VK_ERROR_LAYER_NOT_PRESENT),
        RES_STR(VK_ERROR_EXTENSION_NOT_PRESENT),
        RES_STR(VK_ERROR_FEATURE_NOT_PRESENT),
        RES_STR(VK_ERROR_INCOMPATIBLE_DRIVER),
        RES_STR(VK_ERROR_TOO_MANY_OBJECTS),
        RES_STR(VK_ERROR_FORMAT_NOT_SUPPORTED),
        RES_STR(VK_ERROR_FRAGMENTED_POOL),
        RES_STR(VK_ERROR_UNKNOWN),
        RES_STR(VK_ERROR_OUT_OF_POOL_MEMORY),
        RES_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE),
        RES_STR(VK_ERROR_FRAGMENTATION),
        RES_STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS),
        RES_STR(VK_ERROR_SURFACE_LOST_KHR),
        RES_STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR),
        RES_STR(VK_SUBOPTIMAL_KHR),
        RES_STR(VK_ERROR_OUT_OF_DATE_KHR),
        RES_STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR),
        RES_STR(VK_ERROR_VALIDATION_FAILED_EXT),
        RES_STR(VK_ERROR_INVALID_SHADER_NV),
        RES_STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT),
        RES_STR(VK_ERROR_NOT_PERMITTED_EXT),
        RES_STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT),
        RES_STR(VK_THREAD_IDLE_KHR),
        RES_STR(VK_THREAD_DONE_KHR),
        RES_STR(VK_OPERATION_DEFERRED_KHR),
        RES_STR(VK_OPERATION_NOT_DEFERRED_KHR),
        RES_STR(VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT)
    };

    const char* VkResultToString(VkResult res)
    {
        return vkResultStrings[res];
    }

    void LogVulkanError(VkResult res, const char* functionName)
    {
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("%s failed!, returned %s\n", functionName, VkResultToString(res));
        }
    }

    PFN_vkCmdDrawMeshTasksEXT gPfnDrawMeshTask = nullptr;
    PFN_vkCmdDrawMeshTasksIndirectEXT gPfnDrawMeshTaskIndirect = nullptr;
    PFN_vkCmdDrawMeshTasksIndirectCountEXT gPfnDrawMeshTaskIndirectCount = nullptr;

    SwBool loadMeshShaderExtensions(VkInstance inst)
    {
        gPfnDrawMeshTask = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(
            vkGetInstanceProcAddr(inst, "vkCmdDrawMeshTasksEXT"));
        gPfnDrawMeshTaskIndirect = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectEXT>(
            vkGetInstanceProcAddr(inst, "vkCmdDrawMeshTasksIndirectEXT"));
        gPfnDrawMeshTaskIndirectCount = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountEXT>(
            vkGetInstanceProcAddr(inst, "vkCmdDrawMeshTasksIndirectCountEXT"));

        return (gPfnDrawMeshTask && gPfnDrawMeshTaskIndirect && gPfnDrawMeshTaskIndirectCount);
    }
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ)
{
    if (vk::gPfnDrawMeshTask)
    {
        vk::gPfnDrawMeshTask(commandBuffer, groupCountX, groupCountY, groupCountZ);
    }
}