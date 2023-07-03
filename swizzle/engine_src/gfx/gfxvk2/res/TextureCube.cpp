
/* Include files */

#include "TextureCube.hpp"

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
    TextureCube::TextureCube(common::Resource<Device> device, U32 width, U32 height, U32 channels)
        : mDevice(device)
        , mImage()
        , mImageMemory()
        , mImageView(VK_NULL_HANDLE)
        , mWidth(width)
        , mHeight(height)
        , mChannels(channels)
        , mLayers(6u)
    {
        createResources();
    }

    TextureCube::~TextureCube()
    {
        destroyResources();
    }

    swizzle::gfx::TextureDimensions TextureCube::getSize() const
    {
        return {mWidth, mHeight, 6u, 1u};
    }

    void TextureCube::resize(U32 height, U32 width, U32 channels)
    {
        if (((width != mWidth) || (height != mHeight) || (channels != mChannels)) &&
            ((width != 0U) && (height != 0U) && (channels != 0)))
        {
            destroyResources();
            mWidth = width;
            mHeight = height;
            mChannels = channels;
            createResources();
        }
    }

    void TextureCube::transferImageToCompute(VkCommandBuffer cmdBuffer)
    {
        UNUSED_ARG(cmdBuffer);
    }

    void TextureCube::transferImageToRender(VkCommandBuffer cmdBuffer)
    {
        UNUSED_ARG(cmdBuffer);
    }

    common::Resource<VkResource<VkImage>> TextureCube::getImg()
    {
        return mImage;
    }

    VkImageView TextureCube::getView()
    {
        return mImageView;
    }
    bool TextureCube::isDepth()
    {
        return false;
    }
} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void TextureCube::createImage(VkFormat format)
    {
        VkImageUsageFlags usage =
            VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        mImage = mDevice->createImage(VkImageType::VK_IMAGE_TYPE_2D, format, usage, {mWidth, mHeight, 1u}, mLayers, 1u);
    }

    void TextureCube::allocMemory()
    {
        VkMemoryRequirements reqs;
        vkGetImageMemoryRequirements(mDevice->getDeviceHandle(), mImage->getVkHandle(), &reqs);

        mImageMemory = mDevice->allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, reqs);

        mImageMemory->bind(mDevice, mImage);
    }

    void TextureCube::createView(VkFormat format)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {};

        imageViewCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = VK_NULL_HANDLE;
        imageViewCreateInfo.flags = 0u;
        imageViewCreateInfo.image = mImage->getVkHandle();
        imageViewCreateInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewCreateInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0u;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0u;
        imageViewCreateInfo.subresourceRange.layerCount = mLayers;
        imageViewCreateInfo.subresourceRange.levelCount = 1u;

        // TODO: bind memory before this
        vkCreateImageView(mDevice->getDeviceHandle(), &imageViewCreateInfo, mDevice->getAllocCallbacks(), &mImageView);
    }

    void TextureCube::destroyResources()
    {
        vkDestroyImageView(mDevice->getDeviceHandle(), mImageView, mDevice->getAllocCallbacks());
        mDevice->scheduleResourceDestruction(mImage);
        mDevice->scheduleFreeingMemory(mImageMemory);

        mImageView = VK_NULL_HANDLE;
    }

    void TextureCube::createResources()
    {
        VkFormat selectedFormat;

        switch (mChannels)
        {
        case 1u:
            selectedFormat = VkFormat::VK_FORMAT_R8_SRGB;
            break;
        case 2u:
            selectedFormat = VkFormat::VK_FORMAT_R8G8_SRGB;
            break;
        case 3u:
            selectedFormat = VkFormat::VK_FORMAT_R8G8B8_SRGB;
            break;
        case 4u:
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