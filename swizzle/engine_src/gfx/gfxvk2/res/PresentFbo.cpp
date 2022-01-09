
/* Include files */

#include "PresentFbo.hpp"

#include "Device.hpp"
#include "ShaderPipeline.hpp"

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
    PresentFBO::PresentFBO(common::Resource<Device> device, VkImage image, VkImageView imageView, VkFormat imageFormat,
                           U32 width, U32 height)
        : mDevice(device)
        , mRenderPass(VK_NULL_HANDLE)
        , mFrameBuffer(VK_NULL_HANDLE)
        , mExternalImage(image)
        , mExternalImageView(imageView)
        , mImageFormat(imageFormat)
        , mClearValue()
        , mWidth(width)
        , mHeight(height)
        , mDepthImage(VK_NULL_HANDLE)
        , mDepthImageView(VK_NULL_HANDLE)
        , mDepthMemory()
        , mDepthClearValue()
    {
        createDepthImage();
        mDepthClearValue.depthStencil.depth = 1.0F;
        mDepthClearValue.depthStencil.stencil = 0x0;

        createRenderPass();
        createFramebuffer();
    }

    PresentFBO::~PresentFBO()
    {
        vkDestroyFramebuffer(mDevice->getDeviceHandle(), mFrameBuffer, mDevice->getAllocCallbacks());
        vkDestroyRenderPass(mDevice->getDeviceHandle(), mRenderPass, mDevice->getAllocCallbacks());
        mDevice->freeMemory(mDepthMemory);
        //vkFreeMemory(mDevice->getDeviceHandle(), mDepthMemory, mDevice->getAllocCallbacks());

        vkDestroyImageView(mDevice->getDeviceHandle(), mDepthImageView, mDevice->getAllocCallbacks());
        //vkDestroyImage(mDevice->getDeviceHandle(), mDepthImage, mDevice->getAllocCallbacks());
        common::Resource<IVkResource> res = mDepthImage;
        mDevice->destroyResource(res);
    }

    // Inherited via FrameBuffer
    U32 PresentFBO::getNumColorAttachments() const
    {
        return 1U;
    }

    SwBool PresentFBO::hasDepthAttachment() const
    {
        return false;
    }

    void PresentFBO::setColorAttachmentClearColor(U32 attachmentIndex, swizzle::gfx::ClearColor color)
    {
        UNUSED_ARG(attachmentIndex);
        mClearValue.color.float32[0] = color.r;
        mClearValue.color.float32[1] = color.g;
        mClearValue.color.float32[2] = color.b;
        mClearValue.color.float32[3] = color.a;
    }

    void PresentFBO::setDepthAttachmentClearValue(F32 depthValue, U8 stencilValue)
    {
        UNUSED_ARG(depthValue);
        UNUSED_ARG(stencilValue);
    }

    void PresentFBO::resize(U32 width, U32 height)
    {
        mWidth = width;
        mHeight = height;

        vkDestroyFramebuffer(mDevice->getDeviceHandle(), mFrameBuffer, mDevice->getAllocCallbacks());
        vkDestroyRenderPass(mDevice->getDeviceHandle(), mRenderPass, mDevice->getAllocCallbacks());

        createRenderPass();
        createFramebuffer();
    }

    common::Resource<swizzle::gfx::Shader>
    PresentFBO::createShader(const swizzle::gfx::ShaderAttributeList& attributeList)
    {
        return common::CreateRef<ShaderPipeline>(mDevice, *this, attributeList);
    }

    VkRenderPass PresentFBO::getRenderPass() const
    {
        return mRenderPass;
    }

    VkSampleCountFlagBits PresentFBO::getMultisampleCount() const
    {
        return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
    }

    //void PresentFBO::allocDepthMemory()
    //{
    //    VkMemoryRequirements memreq;
    //    vkGetImageMemoryRequirements(mDevice->getDeviceHandle(), mDepthImage, &memreq);

    //    VkMemoryAllocateInfo allocInfo {};

    //    allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //    allocInfo.pNext = VK_NULL_HANDLE;
    //    allocInfo.allocationSize = memreq.size;
    //    allocInfo.memoryTypeIndex = vk::VulkanMemory::FindMemoryType(
    //        mVkObjects.mPhysicalDevice->getMemoryProperties(),
    //        VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memreq.memoryTypeBits);

    //    vkAllocateMemory(mDevice->getDeviceHandle(), &allocInfo, mVkObjects.mDebugAllocCallbacks,
    //                     &mDepthMemory);

    //    vkBindImageMemory(mVkObjects.mLogicalDevice->getLogical(), mDepthImage, mDepthMemory, 0U);
    //}

    void PresentFBO::createDepthImage()
    {
        //VkImageCreateInfo imageInfo{};

        //imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        //imageInfo.pNext = VK_NULL_HANDLE;
        //imageInfo.flags = 0U;
        //imageInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
        //imageInfo.format = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
        //imageInfo.extent = {mWidth, mHeight, 1U};
        //imageInfo.mipLevels = 1U;
        //imageInfo.arrayLayers = 1U;
        //imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        //imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        //imageInfo.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        //imageInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        //imageInfo.queueFamilyIndexCount = 1u;
        //// imageInfo.pQueueFamilyIndices = &mVkObjects.mQueueFamilyIndex;
        //U32 index = 0u; // TODO: fixme
        //imageInfo.pQueueFamilyIndices = &index;
        //imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED;

        //VkImage depthImg = VK_NULL_HANDLE;
        //vkCreateImage(mDevice->getDeviceHandle(), &imageInfo, mDevice->getAllocCallbacks(), &depthImg);

        mDepthImage = mDevice->createImage(VkImageType::VK_IMAGE_TYPE_2D, VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT,
                                           VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                           {mWidth, mHeight, 1u}, 1u);

        VkMemoryRequirements memreq;
        vkGetImageMemoryRequirements(mDevice->getDeviceHandle(), mDepthImage->getVkHandle(), &memreq);

        mDepthMemory = mDevice->allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memreq);

        //mDepthImage = common::CreateRef<VkResource<VkImage>>(depthImg, ResourceType::ImageResource);

        mDepthMemory->bind(mDevice, mDepthImage);

        VkImageViewCreateInfo imageViewInfo {};

        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.pNext = VK_NULL_HANDLE;
        imageViewInfo.flags = 0U;
        imageViewInfo.image = mDepthImage->getVkHandle();
        imageViewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
        imageViewInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
        imageViewInfo.subresourceRange.baseArrayLayer = 0U;
        imageViewInfo.subresourceRange.baseMipLevel = 0U;
        imageViewInfo.subresourceRange.layerCount = 1U;
        imageViewInfo.subresourceRange.levelCount = 1U;

        vkCreateImageView(mDevice->getDeviceHandle(), &imageViewInfo, mDevice->getAllocCallbacks(),
                          &mDepthImageView);
    }

    void PresentFBO::createRenderPass()
    {
        VkAttachmentReference attach{};
        attach.attachment = 0U;
        attach.layout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;

        VkAttachmentReference depthAttach{};
        depthAttach.attachment = 1U;
        depthAttach.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subPass{};
        subPass.flags = VkSubpassDescriptionFlagBits::VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX;
        subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPass.inputAttachmentCount = 0;
        subPass.pInputAttachments = VK_NULL_HANDLE;
        subPass.colorAttachmentCount = 1;
        subPass.pColorAttachments = &attach;
        subPass.pResolveAttachments = VK_NULL_HANDLE;
        subPass.pDepthStencilAttachment = &depthAttach;
        subPass.preserveAttachmentCount = 0;
        subPass.pPreserveAttachments = VK_NULL_HANDLE;

        subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkAttachmentDescription descr[2]{};

        descr[0].flags = 0;
        descr[0].format = mImageFormat;
        descr[0].samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        descr[0].loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
        descr[0].storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
        descr[0].stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        descr[0].stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        descr[0].initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        descr[0].finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        descr[1].flags = 0;
        descr[1].format = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
        descr[1].samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        descr[1].loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
        descr[1].storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        descr[1].stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
        descr[1].stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        descr[1].initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        descr[1].finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkRenderPassCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = descr;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subPass;
        createInfo.dependencyCount = 0;
        createInfo.pDependencies = VK_NULL_HANDLE;

        VkResult result = vkCreateRenderPass(mDevice->getDeviceHandle(), &createInfo,
                                             mDevice->getAllocCallbacks(), &mRenderPass);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Renderpass creation failed %s", vk::VkResultToString(result));
        }
    }

    void PresentFBO::createFramebuffer()
    {
        mClearValue.color.float32[0] = 0.8F;
        mClearValue.color.float32[1] = 0.6F;
        mClearValue.color.float32[2] = 0.2F;
        mClearValue.color.float32[3] = 1.0F;

        VkFramebufferCreateInfo createInfo{};

        VkImageView views[] = {mExternalImageView, mDepthImageView};

        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.renderPass = mRenderPass;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = views;
        createInfo.width = mWidth;
        createInfo.height = mHeight;
        createInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(mDevice->getDeviceHandle(), &createInfo,
                                              mDevice->getAllocCallbacks(), &mFrameBuffer);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Framebuffer creation failed %s", vk::VkResultToString(result));
        }
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
