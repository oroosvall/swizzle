#include "VulkanTexture.hpp"
#include "backend/VulkanMemory.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace swizzle::gfx
{
    VulkanTexture::VulkanTexture(const VulkanObjectContainer& vkObjects, U32 width, U32 height)
        : mVkObjects(vkObjects)
        , mImage(VK_NULL_HANDLE)
        , mImageView(VK_NULL_HANDLE)
        , mUploaded(false)
        , mWidth(width)
        , mHeight(height)
    {
        S32 x = 0, y = 0, c = 0;

        stbi_uc* pixels = stbi_load("texture/random_snipp.PNG", &x, &y, &c, STBI_rgb_alpha);

        mWidth = static_cast<U32>(x);
        mHeight = static_cast<U32>(y);

        VkDeviceSize imageSize = (U64)mWidth * (U64)mHeight * 4U;

        if (!mVkObjects.stageCmdBuffer->readyToSubmit())
        {
            mVkObjects.stageCmdBuffer->beginStageRecording();
        }

        const auto& stageMemory = mVkObjects.stageCmdBuffer->allocateStagingMemory(pixels, imageSize);

        mStageBuffer = stageMemory.mBuffer;
        mMemory = stageMemory.mMemory;

        stbi_image_free(pixels);

        createResources();
    }

    VulkanTexture::~VulkanTexture()
    {
        destroyResources();
    }

    void VulkanTexture::setData(U32 width, U32 height, U32 channels, U8* pixelData)
    {
        pixelData;
        channels;
        // check to recreate the texture
        if ((width != mWidth) || (height != mHeight) && ((width != 0U) && (height != 0U)) && mVkObjects.stageCmdBuffer->readyToSubmit())
        {
            destroyResources();
            mWidth = width;
            mHeight = height;
            createResources();
        }

        VkDeviceSize imageSize = (U64)mWidth * (U64)mHeight * 4U;

        if (!mVkObjects.stageCmdBuffer->readyToSubmit())
        {
            mVkObjects.stageCmdBuffer->beginStageRecording();
        }

        const auto& stageMemory = mVkObjects.stageCmdBuffer->allocateStagingMemory(pixelData, imageSize);

        mStageBuffer = stageMemory.mBuffer;
        mMemory = stageMemory.mMemory;
        mUploaded = false;
    }

    void VulkanTexture::upload()
    {
        if (mVkObjects.stageCmdBuffer->readyToSubmit())
        {
            uploadImage(mVkObjects.stageCmdBuffer->getCmdBuffer());
        }
    }

    SwBool VulkanTexture::isUploaded() const
    {
        return mUploaded;
    }

    void VulkanTexture::uploadImage(VkCommandBuffer cmdBuffer)
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
        imgBarrier.subresourceRange.layerCount = 1U;

        vkCmdPipelineBarrier(cmdBuffer,
            VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0U, VK_NULL_HANDLE,
            0U, VK_NULL_HANDLE,
            1U, &imgBarrier);

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0U;
        region.bufferImageHeight = 0U;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0U;
        region.imageSubresource.baseArrayLayer = 0U;
        region.imageSubresource.layerCount = 1U;

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

    void VulkanTexture::createImage(VkFormat format)
    {
        VkImageCreateInfo imageCreateInfo = {};

        imageCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = VK_NULL_HANDLE;
        imageCreateInfo.flags = 0;
        imageCreateInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = format;
        imageCreateInfo.extent = { mWidth, mHeight, 1U };
        imageCreateInfo.mipLevels = 1U;
        imageCreateInfo.arrayLayers = 1U;
        imageCreateInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.queueFamilyIndexCount = 1U;
        imageCreateInfo.pQueueFamilyIndices = &mVkObjects.mQueueFamilyIndex;
        imageCreateInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED;

        vkCreateImage(mVkObjects.mLogicalDevice, &imageCreateInfo, nullptr, &mImage);
    }

    void VulkanTexture::allocMemory()
    {
        VkMemoryRequirements memreq;
        vkGetImageMemoryRequirements(mVkObjects.mLogicalDevice, mImage, &memreq);

        VkMemoryAllocateInfo allocInfo = {};

        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = memreq.size;
        allocInfo.memoryTypeIndex = vk::VulkanMemory::FindMemoryType(mVkObjects.mMemoryProperties, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memreq.memoryTypeBits);

        vkAllocateMemory(mVkObjects.mLogicalDevice, &allocInfo, nullptr, &mMemory);

        vkBindImageMemory(mVkObjects.mLogicalDevice, mImage, mMemory, 0U);
    }

    void VulkanTexture::createView(VkFormat format)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {};

        imageViewCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = VK_NULL_HANDLE;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = mImage;
        imageViewCreateInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewCreateInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0U;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0U;
        imageViewCreateInfo.subresourceRange.layerCount = 1U;
        imageViewCreateInfo.subresourceRange.levelCount = 1U;

        // TODO: bind memory before this
        vkCreateImageView(mVkObjects.mLogicalDevice, &imageViewCreateInfo, nullptr, &mImageView);
    }

    void VulkanTexture::destroyResources()
    {
        vkDestroyImageView(mVkObjects.mLogicalDevice, mImageView, nullptr);
        vkFreeMemory(mVkObjects.mLogicalDevice, mMemory, nullptr);
        vkDestroyImage(mVkObjects.mLogicalDevice, mImage, nullptr);

        mImage = VK_NULL_HANDLE;
        mImageView = VK_NULL_HANDLE;
        mMemory = VK_NULL_HANDLE;
    }

    void VulkanTexture::createResources()
    {
        createImage(VkFormat::VK_FORMAT_R8G8B8A8_SRGB);
        allocMemory();
        createView(VkFormat::VK_FORMAT_R8G8B8A8_SRGB);
    }

}
