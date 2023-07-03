
/* Include files */

#include "Texture2D.hpp"

#include "Device.hpp"
#include "DeviceMemory.hpp"

#include <cmath>

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
    Texture2D::Texture2D(common::Resource<Device> device, U32 width, U32 height, U32 channels, SwBool f32)
        : mDevice(device)
        , mImage()
        , mImageMemory()
        , mImageView(VK_NULL_HANDLE)
        , mWidth(width)
        , mHeight(height)
        , mChannels(channels)
        , mMipLevels(1u)
        , mFloat(f32)
    {
        mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1;
        if (mFloat)
        {
            mMipLevels = 1u;
        }
        createResources();

    }

    Texture2D::~Texture2D()
    {
        destroyResources();
    }

    swizzle::gfx::TextureDimensions Texture2D::getSize() const
    {
        return {mWidth, mHeight, 1u, mMipLevels};
    }

    void Texture2D::resize(U32 height, U32 width, U32 channels)
    {
        if (((width != mWidth) || (height != mHeight) || (channels != mChannels)) &&
            ((width != 0U) && (height != 0U) && (channels != 0)))
        {
            destroyResources();
            mWidth = width;
            mHeight = height;
            mChannels = channels;
            mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1;
            createResources();
        }
    }

    void Texture2D::transferImageToCompute(VkCommandBuffer cmdBuffer)
    {
        VkImageMemoryBarrier imgBarrier = {};
        imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgBarrier.pNext = VK_NULL_HANDLE;
        imgBarrier.srcAccessMask = 0u;
        imgBarrier.dstAccessMask =
            VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
        imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.image = mImage->getVkHandle();
        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseMipLevel = 0U;
        imgBarrier.subresourceRange.levelCount = mMipLevels;
        imgBarrier.subresourceRange.baseArrayLayer = 0U;
        imgBarrier.subresourceRange.layerCount = 1U;

        vkCmdPipelineBarrier(cmdBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0U, VK_NULL_HANDLE, 0U,
                             VK_NULL_HANDLE, 1U, &imgBarrier);
    }

    void Texture2D::transferImageToRender(VkCommandBuffer cmdBuffer)
    {
        VkImageMemoryBarrier imgBarrier = {};
        imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgBarrier.pNext = VK_NULL_HANDLE;
        imgBarrier.srcAccessMask =
            VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        imgBarrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        imgBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.image = mImage->getVkHandle();
        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseMipLevel = 0U;
        imgBarrier.subresourceRange.levelCount = mMipLevels;
        imgBarrier.subresourceRange.baseArrayLayer = 0U;
        imgBarrier.subresourceRange.layerCount = 1U;

        vkCmdPipelineBarrier(cmdBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                             VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0U, VK_NULL_HANDLE, 0U,
                             VK_NULL_HANDLE, 1U, &imgBarrier);
    }

    common::Resource<VkResource<VkImage>> Texture2D::getImg()
    {
        return mImage;
    }

    VkImageView Texture2D::getView()
    {
        return mImageView;
    }

    bool Texture2D::isDepth()
    {
        return false;
    }
} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void Texture2D::createImage(VkFormat format)
    {
        VkImageUsageFlags usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                  VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

        if (mFloat)
        {
            usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;
        }

        if (mImage)
        {
            mDevice->scheduleResourceDestruction(mImage);
            mImage.reset();
        }

        mImage =
            mDevice->createImage(VkImageType::VK_IMAGE_TYPE_2D, format, usage, {mWidth, mHeight, 1u}, 1u, mMipLevels);
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
        imageViewCreateInfo.subresourceRange.levelCount = mMipLevels;

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
            if (mFloat)
            {
                selectedFormat = VkFormat::VK_FORMAT_R32_SFLOAT;
            }
            break;
        case 2:
            selectedFormat = VkFormat::VK_FORMAT_R8G8_SRGB;
            break;
        case 3:
            selectedFormat = VkFormat::VK_FORMAT_R8G8B8_SRGB;
            break;
        case 4: {
            selectedFormat = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
            if (mFloat)
            {
                selectedFormat = VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
            }
        }
        break;
        default:
            selectedFormat = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
            break;
        }

        createImage(selectedFormat);
        allocMemory();
        createView(selectedFormat);
    }

    void Texture2D::generateMipMaps(VkCommandBuffer cmdBuffer)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = mImage->getVkHandle();
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = mWidth;
        int32_t mipHeight = mHeight;

        for (uint32_t i = 1; i < mMipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                                 nullptr, 0, nullptr, 1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(cmdBuffer, mImage->getVkHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           mImage->getVkHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                                 nullptr, 0, nullptr, 1, &barrier);

            if (mipWidth > 1)
                mipWidth /= 2;
            if (mipHeight > 1)
                mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mMipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &barrier);
    }
} // namespace vk
