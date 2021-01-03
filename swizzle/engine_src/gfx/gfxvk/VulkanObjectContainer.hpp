#ifndef VULKAN_OBJECT_CONTAINER_HPP
#define VULKAN_OBJECT_CONTAINER_HPP

#include <swizzle/core/Types.hpp>

#include "vk.hpp"
#include "StageCmdBuffer.hpp"

namespace swizzle::gfx
{
    
    // this basically contains all vulkan objects created for the pipeline
    // that get passed around between objects
    // so it is handy to access
    struct VulkanObjectContainer
    {
        VkInstance mInstance;
        VkPhysicalDevice mPhysicalDevice;
        VkPhysicalDeviceMemoryProperties mMemoryProperties;

        VkDevice mLogicalDevice;

        VkQueue mQueue;

        U32 mQueueFamilyIndex;

        VkCommandPool mCmdPool;
        VkPipelineCache mPiplineCache;

        VkDescriptorPool mDescriptorPool;
        StageCmdBuffer* stageCmdBuffer;
    };
}

#endif