#include "VulkanTexture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace swizzle::gfx
{


    VulkanTexture::VulkanTexture(const VulkanObjectContainer& vkObjects)
        : mVkObjects(vkObjects)
        , mImage(VK_NULL_HANDLE)
        , mImageView(VK_NULL_HANDLE)
        , uploaded(false)
    {
        int x = 0, y = 0, c = 0;

        stbi_uc* pixels = stbi_load("texture/random_snipp.PNG", &x, &y, &c, STBI_rgb_alpha);

        mWidth = static_cast<U32>(x);
        mHeight = static_cast<U32>(y);

        VkDeviceSize imageSize = mWidth * mHeight * 4U;

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
        if ((width != mWidth) || (height != mHeight) && ((width != 0U) & (height != 0U)))
        {
            destroyResources();
            mWidth = width;
            mHeight = height;
            createResources();
        }
    }

    void VulkanTexture::frameBufferCreate(U32 width, U32 height, VkFormat format)
    {

    }

    void VulkanTexture::createImage(VkFormat format)
    {
        VkImageCreateInfo imageCreateInfo;

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

        VkMemoryAllocateInfo allocInfo;

        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = memreq.size;
        allocInfo.memoryTypeIndex = vk::FindMemoryType(mVkObjects.mMemoryProperties, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memreq.memoryTypeBits);

        vkAllocateMemory(mVkObjects.mLogicalDevice, &allocInfo, nullptr, &mMemory);

        vkBindImageMemory(mVkObjects.mLogicalDevice, mImage, mMemory, 0U);
    }

    void VulkanTexture::createView(VkFormat format)
    {
        VkImageViewCreateInfo imageViewCreateInfo;

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
