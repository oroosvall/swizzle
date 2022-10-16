
/* Include files */

#include "VFrameBuffer.hpp"
#include "Device.hpp"
#include "ShaderPipeline.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace vk
{
    common::Resource<BaseFrameBuffer>
        GetFrameBufferAsBaseFrameBuffer(common::Resource<swizzle::gfx::FrameBuffer> buffer)
    {
        return std::dynamic_pointer_cast<BaseFrameBuffer>(buffer);
    }
} // namespace vk

/* Class Public Function Definition */

namespace vk
{
    VFrameBuffer::VFrameBuffer(common::Resource<Device> device, const swizzle::gfx::FrameBufferCreateInfo& createInfo)
        : mDevice(device)
        , mCreateInfo(createInfo)
        , mRenderPass(VK_NULL_HANDLE)
        , mFrameBuffer(VK_NULL_HANDLE)
        , mWidth(createInfo.mWidth)
        , mHeight(createInfo.mHeight)
        , mHasDepthBuffer(false)
        , mDepthImage()
        , mImageFormat(VkFormat::VK_FORMAT_R8G8B8A8_UNORM)
        , mImages()
    {
        if (mWidth == 0u)
        {
            mWidth = 1u;
        }
        if (mHeight == 0u)
        {
            mHeight = 1u;
        }

        if (createInfo.mDepthType != swizzle::gfx::FrameBufferDepthType::DepthNone)
        {
            mHasDepthBuffer = true;
        }

        if (createInfo.mAttachmentType == swizzle::gfx::FrameBufferAttachmentType::F32)
        {
            mImageFormat = VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
        }

        createColorImages();
        createDepthImage();
        createRenderPass();
        createFramebuffer();
    }

    VFrameBuffer::~VFrameBuffer()
    {
        destroyResources();
    }

    U32 VFrameBuffer::getNumColorAttachments() const
    {
        return static_cast<U32>(mImages.size());
    }

    SwBool VFrameBuffer::hasDepthAttachment() const
    {
        return mHasDepthBuffer;
    }

    void VFrameBuffer::setColorAttachmentClearColor(U32 attachmentIndex, swizzle::gfx::ClearColor color)
    {
        if (attachmentIndex < getNumColorAttachments())
        {
            mImages[attachmentIndex].mClearValue.color = {color.r, color.g, color.b, color.a};
        }
        else
        {
            LOG_ERROR("Invalid color attachment index");
        }
    }

    void VFrameBuffer::setDepthAttachmentClearValue(F32 depthValue, U8 stencilValue)
    {
        if (mHasDepthBuffer)
        {
            mDepthImage.mClearValue.depthStencil.depth = depthValue;
            mDepthImage.mClearValue.depthStencil.stencil = stencilValue;
        }
        else
        {
            LOG_ERROR("Framebuffer is missing depth attachment");
        }
    }

    common::Resource<swizzle::gfx::Texture> VFrameBuffer::getColorAttachment(U32 index)
    {
        return common::CreateRef<DummyTexture>(mImages[index].mImage, mImages[index].mImageView);
    }

    void VFrameBuffer::resize(U32 width, U32 height)
    {
        mDevice->waitDeviceIdle();

        mWidth = width;
        mHeight = height;

        if (mWidth == 0u)
        {
            mWidth = 1u;
        }
        if (mHeight == 0u)
        {
            mHeight = 1u;
        }

        destroyResources();

        createColorImages();
        createDepthImage();
        createRenderPass();
        createFramebuffer();
    }

    VkRenderPass VFrameBuffer::getRenderPass() const
    {
        return mRenderPass;
    }

    VkSampleCountFlagBits VFrameBuffer::getMultisampleCount() const
    {
        return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
    }

    VkFramebuffer VFrameBuffer::getFrameBuffer() const
    {
        return mFrameBuffer;
    }

    void VFrameBuffer::getSize(U32& width, U32& height) const
    {
        width = mWidth;
        height = mHeight;
    }

    VkClearValue VFrameBuffer::getColorClearValue(U32 index) const
    {
        VkClearValue color{};
        if (index < getNumColorAttachments())
        {
            color = mImages[index].mClearValue;
        }
        else
        {
            color.color.float32[0] = 0.0f;
            color.color.float32[1] = 0.0f;
            color.color.float32[2] = 0.0f;
            color.color.float32[3] = 0.0f;
            LOG_ERROR("Invalid color attachment index");
        }
        return color;
    }

    VkClearValue VFrameBuffer::getDepthClearValue() const
    {
        return mDepthImage.mClearValue;
    }

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void VFrameBuffer::destroyResources()
    {
        vkDestroyFramebuffer(mDevice->getDeviceHandle(), mFrameBuffer, mDevice->getAllocCallbacks());
        vkDestroyRenderPass(mDevice->getDeviceHandle(), mRenderPass, mDevice->getAllocCallbacks());

        for (auto& img : mImages)
        {
            common::Resource<IVkResource> res = img.mImage;
            mDevice->destroyResource(res);
            mDevice->scheduleFreeingMemory(img.mImageMemory);
            vkDestroyImageView(mDevice->getDeviceHandle(), img.mImageView, mDevice->getAllocCallbacks());
        }
        mImages.clear();

        if (mHasDepthBuffer)
        {
            common::Resource<IVkResource> res = mDepthImage.mImage;
            mDevice->destroyResource(res);
            mDevice->scheduleFreeingMemory(mDepthImage.mImageMemory);
            vkDestroyImageView(mDevice->getDeviceHandle(), mDepthImage.mImageView, mDevice->getAllocCallbacks());

            mDepthImage.mImage.reset();
            mDepthImage.mImageMemory.reset();
        }
    }

    void VFrameBuffer::createColorImages()
    {
        for (U32 i = 0u; i < mCreateInfo.mNumColAttach; ++i)
        {

            common::Resource<VkResource<VkImage>> image = mDevice->createImage(
                VkImageType::VK_IMAGE_TYPE_2D, mImageFormat,
                VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                    VkImageUsageFlagBits::VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                {mWidth, mHeight, 1u}, 1u);

            VkMemoryRequirements memreq;
            vkGetImageMemoryRequirements(mDevice->getDeviceHandle(), image->getVkHandle(), &memreq);

            common::Resource<DeviceMemory> memory =
                mDevice->allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memreq);

            memory->bind(mDevice, image);

            VkImageViewCreateInfo imageViewInfo{};

            imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewInfo.pNext = VK_NULL_HANDLE;
            imageViewInfo.flags = 0U;
            imageViewInfo.image = image->getVkHandle();
            imageViewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
            imageViewInfo.format = mImageFormat;
            imageViewInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;

            imageViewInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewInfo.subresourceRange.baseArrayLayer = 0u;
            imageViewInfo.subresourceRange.baseMipLevel = 0u;
            imageViewInfo.subresourceRange.layerCount = 1u;
            imageViewInfo.subresourceRange.levelCount = 1u;

            VkImageView imageView = VK_NULL_HANDLE;

            vkCreateImageView(mDevice->getDeviceHandle(), &imageViewInfo, mDevice->getAllocCallbacks(), &imageView);

            VkClearColorValue clearColor{};

            clearColor.float32[0] = 0.0f;
            clearColor.float32[1] = 0.0f;
            clearColor.float32[2] = 0.0f;
            clearColor.float32[3] = 0.0f;

            mImages.emplace_back(VFrameBufferImage{image, imageView, memory, clearColor});
        }
    }

    void VFrameBuffer::createDepthImage()
    {
        if (mHasDepthBuffer)
        {

            mDepthImage.mImage = mDevice->createImage(
                VkImageType::VK_IMAGE_TYPE_2D, VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT,
                VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, {mWidth, mHeight, 1u}, 1u);

            VkMemoryRequirements memreq;
            vkGetImageMemoryRequirements(mDevice->getDeviceHandle(), mDepthImage.mImage->getVkHandle(), &memreq);

            mDepthImage.mImageMemory =
                mDevice->allocateMemory(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memreq);

            mDepthImage.mImageMemory->bind(mDevice, mDepthImage.mImage);

            VkImageViewCreateInfo imageViewInfo{};

            imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewInfo.pNext = VK_NULL_HANDLE;
            imageViewInfo.flags = 0u;
            imageViewInfo.image = mDepthImage.mImage->getVkHandle();
            imageViewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
            imageViewInfo.format = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
            imageViewInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;

            imageViewInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
            imageViewInfo.subresourceRange.baseArrayLayer = 0u;
            imageViewInfo.subresourceRange.baseMipLevel = 0u;
            imageViewInfo.subresourceRange.layerCount = 1u;
            imageViewInfo.subresourceRange.levelCount = 1u;

            vkCreateImageView(mDevice->getDeviceHandle(), &imageViewInfo, mDevice->getAllocCallbacks(),
                              &mDepthImage.mImageView);
        }
    }

    void VFrameBuffer::createRenderPass()
    {
        std::vector<VkAttachmentReference> attachments;
        U32 counter = 0u;
        for (; counter < static_cast<U32>(mImages.size()); ++counter)
        {
            attachments.emplace_back(VkAttachmentReference{counter, VkImageLayout::VK_IMAGE_LAYOUT_GENERAL});
        }

        VkAttachmentReference* depthAttach = VK_NULL_HANDLE;
        if (mHasDepthBuffer)
        {
            attachments.emplace_back(
                VkAttachmentReference{counter, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL});
            depthAttach = &attachments[attachments.size() - 1u];
        }

        VkSubpassDescription subPass{};
        subPass.flags = VkSubpassDescriptionFlagBits::VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX;
        subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPass.inputAttachmentCount = 0u;
        subPass.pInputAttachments = VK_NULL_HANDLE;
        subPass.colorAttachmentCount = static_cast<U32>(mImages.size());
        subPass.pColorAttachments = attachments.data();
        subPass.pResolveAttachments = VK_NULL_HANDLE;
        subPass.pDepthStencilAttachment = depthAttach;
        subPass.preserveAttachmentCount = 0u;
        subPass.pPreserveAttachments = VK_NULL_HANDLE;

        subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;

        std::vector<VkAttachmentDescription> attachDescr;

        for (U32 i = 0u; i < static_cast<U32>(mImages.size()); ++i)
        {
            VkAttachmentDescription desc{};

            desc.flags = 0u;
            desc.format = mImageFormat;
            desc.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
            desc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
            desc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
            desc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            desc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
            desc.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
            desc.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            attachDescr.emplace_back(desc);
        }

        if (mHasDepthBuffer)
        {
            VkAttachmentDescription desc{};

            desc.flags = 0u;
            desc.format = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
            desc.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
            desc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
            desc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
            desc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
            desc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
            desc.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
            desc.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            // desc.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

            attachDescr.emplace_back(desc);
        }

        VkRenderPassCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0u;
        createInfo.attachmentCount = static_cast<U32>(attachDescr.size());
        createInfo.pAttachments = attachDescr.data();
        createInfo.subpassCount = 1u;
        createInfo.pSubpasses = &subPass;
        createInfo.dependencyCount = 0u;
        createInfo.pDependencies = VK_NULL_HANDLE;

        VkResult result =
            vkCreateRenderPass(mDevice->getDeviceHandle(), &createInfo, mDevice->getAllocCallbacks(), &mRenderPass);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Renderpass creation failed %s", vk::VkResultToString(result));
        }
    }

    void VFrameBuffer::createFramebuffer()
    {
        VkFramebufferCreateInfo createInfo{};

        std::vector<VkImageView> views;

        for (auto& imgs : mImages)
        {
            views.push_back(imgs.mImageView);
        }

        if (mHasDepthBuffer)
        {
            views.push_back(mDepthImage.mImageView);
        }

        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0u;
        createInfo.renderPass = mRenderPass;
        createInfo.attachmentCount = static_cast<U32>(views.size());
        createInfo.pAttachments = views.data();
        createInfo.width = mWidth;
        createInfo.height = mHeight;
        createInfo.layers = 1u;

        VkResult result =
            vkCreateFramebuffer(mDevice->getDeviceHandle(), &createInfo, mDevice->getAllocCallbacks(), &mFrameBuffer);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Framebuffer creation failed %s", vk::VkResultToString(result));
        }
    }
} // namespace vk
