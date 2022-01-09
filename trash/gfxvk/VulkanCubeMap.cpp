
/* Include files */

#include "VulkanDevice.hpp"

#include "VulkanCubeMap.hpp"
#include "backend/VulkanMemory.hpp"

#include <cstring>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle::gfx
{
    VulkanCubeMap::VulkanCubeMap(const VkContainer vkObjects, U32 width, U32 height, U32 channels, const U8* pixelData)
        : mVkObjects(vkObjects)
        , mImage(VK_NULL_HANDLE)
        , mImageView(VK_NULL_HANDLE)
        , mMemory(VK_NULL_HANDLE)
        , mUploaded(false)
        , mStageBuffer(VK_NULL_HANDLE)
        , mStageMemory(VK_NULL_HANDLE)
        , mWidth(width)
        , mHeight(height)
        , mChannels(channels)
        , mLayers(6u)
    {
        createResources();

        if (pixelData)
        {
            setData(width, height, channels, pixelData);
        }
    }

    VulkanCubeMap::~VulkanCubeMap()
    {
        destroyResources();

        if (mStageBuffer)
        {
            vkDestroyBuffer(mVkObjects.mLogicalDevice->getLogical(), mStageBuffer, nullptr);
            vkFreeMemory(mVkObjects.mLogicalDevice->getLogical(), mStageMemory, nullptr);

            mStageBuffer = VK_NULL_HANDLE;
            mStageMemory = VK_NULL_HANDLE;
        }
    }

    void VulkanCubeMap::setData(U32 width, U32 height, U32 channels, const U8* pixelData)
    {
        mChannels = channels;
        // check to recreate the texture
        if (((width != mWidth) || (height != mHeight)) && ((width != 0U) && (height != 0U)) /*&& mVkObjects.stageCmdBuffer->readyToSubmit()*/)
        {
            destroyResources();
            mWidth = width;
            mHeight = height;
            createResources();
        }

        if (mStageBuffer)
        {
            vkDestroyBuffer(mVkObjects.mLogicalDevice->getLogical(), mStageBuffer, nullptr);
            vkFreeMemory(mVkObjects.mLogicalDevice->getLogical(), mStageMemory, nullptr);

            mStageBuffer = VK_NULL_HANDLE;
            mStageMemory = VK_NULL_HANDLE;
        }

        const U32 queueIndex = VK_QUEUE_FAMILY_IGNORED;

        VkDeviceSize imageSize = (U64)mWidth * (U64)mHeight * (U64)channels * (U64)mLayers;

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = VK_NULL_HANDLE;
        bufferInfo.flags = 0;
        bufferInfo.size = imageSize;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = 1u;
        bufferInfo.pQueueFamilyIndices = &queueIndex;

        vkCreateBuffer(mVkObjects.mLogicalDevice->getLogical(), &bufferInfo, nullptr, &mStageBuffer);

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(mVkObjects.mLogicalDevice->getLogical(), mStageBuffer, &req);

        VkMemoryAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = req.size;
        allocInfo.memoryTypeIndex = vk::VulkanMemory::FindMemoryType(mVkObjects.mPhysicalDevice->getMemoryProperties(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, req.memoryTypeBits);

        vkAllocateMemory(mVkObjects.mLogicalDevice->getLogical(), &allocInfo, nullptr, &mStageMemory);

        vkBindBufferMemory(mVkObjects.mLogicalDevice->getLogical(), mStageBuffer, mStageMemory, 0U);

        void* dataPtr = nullptr;
        vkMapMemory(mVkObjects.mLogicalDevice->getLogical(), mStageMemory, 0, imageSize, 0, &dataPtr);
        memcpy(dataPtr, pixelData, static_cast<size_t>(imageSize));
        vkUnmapMemory(mVkObjects.mLogicalDevice->getLogical(), mStageMemory);

        mUploaded = false;
    }

    void VulkanCubeMap::upload()
    {

    }

    VkImageView VulkanCubeMap::getView() const
    {
        return mImageView;
    }

    SwBool VulkanCubeMap::isUploaded() const
    {
        return mUploaded;
    }

    void VulkanCubeMap::uploadImage(VkCommandBuffer cmdBuffer)
    {
        VkImageMemoryBarrier imgBarrier = {};
        imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgBarrier.pNext = VK_NULL_HANDLE;
        imgBarrier.srcAccessMask = 0;
        imgBarrier.dstAccessMask = 0;
        imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.image = mImage;
        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseMipLevel = 0U;
        imgBarrier.subresourceRange.levelCount = 1U;
        imgBarrier.subresourceRange.baseArrayLayer = 0U;
        imgBarrier.subresourceRange.layerCount = mLayers;

        vkCmdPipelineBarrier(cmdBuffer,
            VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0U, VK_NULL_HANDLE,
            0U, VK_NULL_HANDLE,
            1U, &imgBarrier);

        VkBufferImageCopy region = {};
        region.bufferOffset = 0u;
        region.bufferRowLength = mWidth;
        region.bufferImageHeight = mHeight;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0U;
        region.imageSubresource.baseArrayLayer = 0U;
        region.imageSubresource.layerCount = mLayers;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            mWidth,
            mHeight,
            1
        };

        vkCmdCopyBufferToImage(cmdBuffer, mStageBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1U,
            &region);

        imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        vkCmdPipelineBarrier(cmdBuffer,
            VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0U, VK_NULL_HANDLE,
            0U, VK_NULL_HANDLE,
            1U, &imgBarrier);

        mUploaded = true;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace swizzle::gfx
{
    void VulkanCubeMap::createImage(VkFormat format)
    {
        VkImageCreateInfo imageCreateInfo = {};

        imageCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = VK_NULL_HANDLE;
        imageCreateInfo.flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        imageCreateInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = format;
        imageCreateInfo.extent = { mWidth, mHeight, 1U };
        imageCreateInfo.mipLevels = 1U;
        imageCreateInfo.arrayLayers = mLayers;
        imageCreateInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.queueFamilyIndexCount = 1U;
        //imageCreateInfo.pQueueFamilyIndices = &mVkObjects.mQueueFamilyIndex;
        U32 idx = 0u; // TODO: fixme
        imageCreateInfo.pQueueFamilyIndices = &idx;
        imageCreateInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED;

        vkCreateImage(mVkObjects.mLogicalDevice->getLogical(), &imageCreateInfo, mVkObjects.mDebugAllocCallbacks, &mImage);
    }

    void VulkanCubeMap::allocMemory()
    {
        VkMemoryRequirements memreq;
        vkGetImageMemoryRequirements(mVkObjects.mLogicalDevice->getLogical(), mImage, &memreq);

        VkMemoryAllocateInfo allocInfo = {};

        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = memreq.size;
        allocInfo.memoryTypeIndex = vk::VulkanMemory::FindMemoryType(mVkObjects.mPhysicalDevice->getMemoryProperties(), VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memreq.memoryTypeBits);

        vkAllocateMemory(mVkObjects.mLogicalDevice->getLogical(), &allocInfo, mVkObjects.mDebugAllocCallbacks, &mMemory);

        vkBindImageMemory(mVkObjects.mLogicalDevice->getLogical(), mImage, mMemory, 0U);
    }

    void VulkanCubeMap::createView(VkFormat format)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {};

        imageViewCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = VK_NULL_HANDLE;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = mImage;
        imageViewCreateInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewCreateInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0U;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0U;
        imageViewCreateInfo.subresourceRange.layerCount = mLayers;
        imageViewCreateInfo.subresourceRange.levelCount = 1U;

        // TODO: bind memory before this
        vkCreateImageView(mVkObjects.mLogicalDevice->getLogical(), &imageViewCreateInfo, mVkObjects.mDebugAllocCallbacks, &mImageView);
    }

    void VulkanCubeMap::destroyResources()
    {
        vkDestroyImageView(mVkObjects.mLogicalDevice->getLogical(), mImageView, mVkObjects.mDebugAllocCallbacks);
        vkFreeMemory(mVkObjects.mLogicalDevice->getLogical(), mMemory, mVkObjects.mDebugAllocCallbacks);
        vkDestroyImage(mVkObjects.mLogicalDevice->getLogical(), mImage, mVkObjects.mDebugAllocCallbacks);

        mImage = VK_NULL_HANDLE;
        mImageView = VK_NULL_HANDLE;
        mMemory = VK_NULL_HANDLE;
    }

    void VulkanCubeMap::createResources()
    {
        VkFormat selectedFormat;

        switch (mChannels)
        {
        case 1:
            selectedFormat = VkFormat::VK_FORMAT_R8_SRGB;
            break;
        case 2:
            selectedFormat = VkFormat::VK_FORMAT_R8G8_SRGB;
            break;
        case 3:
            selectedFormat = VkFormat::VK_FORMAT_R8G8B8_SRGB;
            break;
        case 4:
            selectedFormat = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
            break;
        default:
            selectedFormat = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
            break;
        }

        createImage(selectedFormat);
        allocMemory();
        createView(selectedFormat);
    }
}