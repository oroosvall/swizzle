
/* Include files */

#include "Texture2D.hpp"

#include "Device.hpp"
#include "DeviceMemory.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace vk
{
    Texture2D::Texture2D(common::Resource<Device> device, U32 width, U32 height, U32 channels, const U8* pixelData)
        : mDevice(device)
        , mImage()
        , mImageMemory()
        , mImageView(VK_NULL_HANDLE)
        , mUploaded(true)
        , mStageBuffer()
        , mStageMemory()
        , mWidth(width)
        , mHeight(height)
        , mChannels(channels)
    {
        createResources();

        if (pixelData)
        {
            setData(width, height, channels, pixelData);
        }
    }

    Texture2D::~Texture2D()
    {
        destroyResources();

        if (mStageBuffer)
        {
            common::Resource<IVkResource> res = mStageBuffer;
            mDevice->destroyResource(res);
            mDevice->freeMemory(mStageMemory);

            mStageBuffer.reset();
            mStageMemory.reset();
        }
    }

    void Texture2D::setData(U32 width, U32 height, U32 channels, const U8* pixelData)
    {
        mChannels = channels;
        // check to recreate the texture
        if (((width != mWidth) || (height != mHeight)) &&
            ((width != 0U) && (height != 0U)) /*&& mVkObjects.stageCmdBuffer->readyToSubmit()*/)
        {
            destroyResources();
            mWidth = width;
            mHeight = height;
            createResources();
        }

        if (mStageBuffer)
        {
            common::Resource<IVkResource> res = mStageBuffer;
            mDevice->destroyResource(res);
            mDevice->freeMemory(mStageMemory);

            mStageBuffer.reset();
            mStageMemory.reset();
        }

        VkDeviceSize imageSize = (U64)mWidth * (U64)mHeight * (U64)channels;

        mStageBuffer = mDevice->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(mDevice->getDeviceHandle(), mStageBuffer->getVkHandle(), &req);

        mStageMemory =
            mDevice->allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, req);

        mStageMemory->bind(mDevice, mStageBuffer);

        void* dataPtr = nullptr;
        vkMapMemory(mDevice->getDeviceHandle(), mStageMemory->mMemory, mStageMemory->mAlignOffset, imageSize, 0,
                    &dataPtr);
        memcpy(dataPtr, pixelData, static_cast<size_t>(imageSize));
        vkUnmapMemory(mDevice->getDeviceHandle(), mStageMemory->mMemory);

        mUploaded = false;
    }

    void Texture2D::getTextureSize(U32& w, U32& h)
    {
        w = mWidth;
        h = mHeight;
    }

    void Texture2D::upload() {}

    SwBool Texture2D::isUploaded() const
    {
        return mUploaded;
    }

    void Texture2D::uploadImage(VkCommandBuffer cmdBuffer)
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
        imgBarrier.image = mImage->getVkHandle();
        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseMipLevel = 0U;
        imgBarrier.subresourceRange.levelCount = 1U;
        imgBarrier.subresourceRange.baseArrayLayer = 0U;
        imgBarrier.subresourceRange.layerCount = 1U;

        vkCmdPipelineBarrier(cmdBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0U, VK_NULL_HANDLE, 0U,
                             VK_NULL_HANDLE, 1U, &imgBarrier);

        VkBufferImageCopy region = {};
        region.bufferOffset = 0u;
        region.bufferRowLength = mWidth;
        region.bufferImageHeight = mHeight;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0U;
        region.imageSubresource.baseArrayLayer = 0U;
        region.imageSubresource.layerCount = 1U;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {mWidth, mHeight, 1};

        vkCmdCopyBufferToImage(cmdBuffer, mStageBuffer->getVkHandle(), mImage->getVkHandle(),
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1U, &region);

        imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        vkCmdPipelineBarrier(cmdBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0U, VK_NULL_HANDLE, 0U,
                             VK_NULL_HANDLE, 1U, &imgBarrier);

        mUploaded = true;
    }

    common::Resource<VkResource<VkImage>> Texture2D::getImg()
    {
        return mImage;
    }

    VkImageView Texture2D::getView()
    {
        return mImageView;
    }
} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void Texture2D::createImage(VkFormat format)
    {
        VkImageUsageFlags usage =
            VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        if (mImage)
        {
            mDevice->scheduleResourceDestruction(mImage);
            mImage.reset();
        }

        mImage = mDevice->createImage(VkImageType::VK_IMAGE_TYPE_2D, format, usage, {mWidth, mHeight, 1u}, 1u);
    }

    void Texture2D::allocMemory()
    {
        VkMemoryRequirements reqs;
        vkGetImageMemoryRequirements(mDevice->getDeviceHandle(), mImage->getVkHandle(), &reqs);

        if (mImageMemory)
        {
            mDevice->scheduleFreeingMemory(mImageMemory);
            mImageMemory.reset();
        }

        mImageMemory = mDevice->allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, reqs);

        mImageMemory->bind(mDevice, mImage);
    }

    void Texture2D::createView(VkFormat format)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {};

        imageViewCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = VK_NULL_HANDLE;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = mImage->getVkHandle();
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
        vkCreateImageView(mDevice->getDeviceHandle(), &imageViewCreateInfo, mDevice->getAllocCallbacks(), &mImageView);
    }

    void Texture2D::destroyResources()
    {
        vkDestroyImageView(mDevice->getDeviceHandle(), mImageView, mDevice->getAllocCallbacks());
        mDevice->scheduleResourceDestruction(mImage);
        mDevice->scheduleFreeingMemory(mImageMemory);
        /*vkFreeMemory(mVkObjects.mLogicalDevice->getLogical(), mMemory, mVkObjects.mDebugAllocCallbacks);
        vkDestroyImage(mVkObjects.mLogicalDevice->getLogical(), mImage, mVkObjects.mDebugAllocCallbacks);*/

        mImageView = VK_NULL_HANDLE;
        // mImage = VK_NULL_HANDLE;
        // mMemory = VK_NULL_HANDLE;
    }

    void Texture2D::createResources()
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
} // namespace vk
