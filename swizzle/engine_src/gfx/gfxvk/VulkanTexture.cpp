#include "VulkanPhysicalDevice.hpp"

#include "VulkanTexture.hpp"
#include "backend/VulkanMemory.hpp"

#include <stb/stb_image.h>

namespace swizzle::gfx
{
    VulkanTexture::VulkanTexture(const VkContainer vkObjects, U32 width, U32 height)
        : mVkObjects(vkObjects)
        , mImage(VK_NULL_HANDLE)
        , mImageView(VK_NULL_HANDLE)
        , mUploaded(true) // Black texture by default
        , mWidth(width)
        , mHeight(height)
        , mStageBuffer(VK_NULL_HANDLE)
        , mStageMemory(VK_NULL_HANDLE)
    {
        createResources();

        // TODO: remove me
        S32 x = 0, y = 0, c = 0;

        stbi_uc* pixels = stbi_load("texture/lightGray.PNG", &x, &y, &c, STBI_rgb_alpha);

        setData(x, y, 4u, pixels);

        stbi_image_free(pixels);
    }

    VulkanTexture::~VulkanTexture()
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

    void VulkanTexture::setData(U32 width, U32 height, U32 channels, U8* pixelData)
    {
        pixelData;
        mChannels = channels;
        // check to recreate the texture
        if ((width != mWidth) || (height != mHeight) && ((width != 0U) && (height != 0U)) /*&& mVkObjects.stageCmdBuffer->readyToSubmit()*/)
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

        VkDeviceSize imageSize = (U64)mWidth * (U64)mHeight * (U64)channels;

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
        allocInfo.memoryTypeIndex = vk::VulkanMemory2::FindMemoryType(mVkObjects.mPhysicalDevice->getMemoryProperties(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, req.memoryTypeBits);

        vkAllocateMemory(mVkObjects.mLogicalDevice->getLogical(), &allocInfo, nullptr, &mStageMemory);

        vkBindBufferMemory(mVkObjects.mLogicalDevice->getLogical(), mStageBuffer, mStageMemory, 0U);

        void* dataPtr = nullptr;
        vkMapMemory(mVkObjects.mLogicalDevice->getLogical(), mStageMemory, 0, imageSize, 0, &dataPtr);
        memcpy(dataPtr, pixelData, static_cast<size_t>(imageSize));
        vkUnmapMemory(mVkObjects.mLogicalDevice->getLogical(), mStageMemory);


        /*if (!mVkObjects.stageCmdBuffer->readyToSubmit())
        {
            mVkObjects.stageCmdBuffer->beginStageRecording();
        }

        const auto& stageMemory = mVkObjects.stageCmdBuffer->allocateStagingMemory(pixelData, imageSize);

        mStageBuffer = stageMemory.mBuffer;
        mMemory = stageMemory.mMemory;*/
        mUploaded = false;
    }

    void VulkanTexture::upload()
    {
        /*if (mVkObjects.stageCmdBuffer->readyToSubmit())
        {
            uploadImage(mVkObjects.stageCmdBuffer->getCmdBuffer());
        }*/
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
        region.bufferOffset = 0u;
        region.bufferRowLength = mWidth;
        region.bufferImageHeight = mHeight;

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
        //imageCreateInfo.pQueueFamilyIndices = &mVkObjects.mQueueFamilyIndex;
        U32 idx = 0u; // TODO: fixme
        imageCreateInfo.pQueueFamilyIndices = &idx;
        imageCreateInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED;

        vkCreateImage(mVkObjects.mLogicalDevice->getLogical(), &imageCreateInfo, mVkObjects.mDebugAllocCallbacks, &mImage);
    }

    void VulkanTexture::allocMemory()
    {
        VkMemoryRequirements memreq;
        vkGetImageMemoryRequirements(mVkObjects.mLogicalDevice->getLogical(), mImage, &memreq);

        VkMemoryAllocateInfo allocInfo = {};

        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = memreq.size;
        allocInfo.memoryTypeIndex = vk::VulkanMemory2::FindMemoryType(mVkObjects.mPhysicalDevice->getMemoryProperties(), VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memreq.memoryTypeBits);

        vkAllocateMemory(mVkObjects.mLogicalDevice->getLogical(), &allocInfo, mVkObjects.mDebugAllocCallbacks, &mMemory);

        vkBindImageMemory(mVkObjects.mLogicalDevice->getLogical(), mImage, mMemory, 0U);
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
        vkCreateImageView(mVkObjects.mLogicalDevice->getLogical(), &imageViewCreateInfo, mVkObjects.mDebugAllocCallbacks, &mImageView);
    }

    void VulkanTexture::destroyResources()
    {
        vkDestroyImageView(mVkObjects.mLogicalDevice->getLogical(), mImageView, mVkObjects.mDebugAllocCallbacks);
        vkFreeMemory(mVkObjects.mLogicalDevice->getLogical(), mMemory, mVkObjects.mDebugAllocCallbacks);
        vkDestroyImage(mVkObjects.mLogicalDevice->getLogical(), mImage, mVkObjects.mDebugAllocCallbacks);

        mImage = VK_NULL_HANDLE;
        mImageView = VK_NULL_HANDLE;
        mMemory = VK_NULL_HANDLE;
    }

    void VulkanTexture::createResources()
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
