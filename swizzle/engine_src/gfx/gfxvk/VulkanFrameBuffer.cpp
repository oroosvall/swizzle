
#include "VulkanFramebuffer.hpp"
#include "shader/VulkanShader.hpp"
#include "backend/VulkanMemory.hpp"
#include <swizzle/core/Logging.hpp>

namespace swizzle::gfx
{

    PresentFrameBuffer::PresentFrameBuffer(const VulkanObjectContainer& vkObjects, VkImage image, VkImageView imageView, VkFormat imageFormat, U32 width, U32 height)
        : mVkObjects(vkObjects)
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
        , mDepthMemory(VK_NULL_HANDLE)
        , mDepthClearValue()
    {
        createDepthImage();
        mDepthClearValue.depthStencil.depth = 1.0F;
        mDepthClearValue.depthStencil.stencil = 0x0;

        createRenderPass();
        createFramebuffer();
    }

    PresentFrameBuffer::~PresentFrameBuffer()
    {
        vkDestroyFramebuffer(mVkObjects.mLogicalDevice, mFrameBuffer, mVkObjects.mDebugAllocCallbacks);
        vkDestroyRenderPass(mVkObjects.mLogicalDevice, mRenderPass, mVkObjects.mDebugAllocCallbacks);
        vkFreeMemory(mVkObjects.mLogicalDevice, mDepthMemory, mVkObjects.mDebugAllocCallbacks);

        vkDestroyImageView(mVkObjects.mLogicalDevice, mDepthImageView, mVkObjects.mDebugAllocCallbacks);
        vkDestroyImage(mVkObjects.mLogicalDevice, mDepthImage, mVkObjects.mDebugAllocCallbacks);

    }

    // Inherited via FrameBuffer
    U32 PresentFrameBuffer::getNumColorAttachments() const
    {
        return 1U;
    }

    SwBool PresentFrameBuffer::hasDepthAttachment() const
    {
        return false;
    }

    void PresentFrameBuffer::setColorAttachmentClearColor(U32 attachmentIndex, ClearColor color)
    {
        attachmentIndex;
        mClearValue.color.float32[0] = color.r;
        mClearValue.color.float32[1] = color.g;
        mClearValue.color.float32[2] = color.b;
        mClearValue.color.float32[3] = color.a;
    }

    void PresentFrameBuffer::setDepthAttachmentClearValue(F32 depthValue, U8 stencilValue)
    {
        depthValue;
        stencilValue;
    }

    void PresentFrameBuffer::resize(U32 width, U32 height)
    {
        mWidth = width;
        mHeight = height;

        vkDestroyFramebuffer(mVkObjects.mLogicalDevice, mFrameBuffer, mVkObjects.mDebugAllocCallbacks);
        vkDestroyRenderPass(mVkObjects.mLogicalDevice, mRenderPass, mVkObjects.mDebugAllocCallbacks);

        createRenderPass();
        createFramebuffer();
    }

    core::Resource<Shader> PresentFrameBuffer::createShader(const ShaderAttributeList& attributeList)
    {
        return core::CreateRef<VulkanShader>(mVkObjects, *this, attributeList);
    }

    VkRenderPass PresentFrameBuffer::getRenderPass() const
    {
        return mRenderPass;
    }

    VkSampleCountFlagBits PresentFrameBuffer::getMultisampleCount() const
    {
        return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
    }

    void PresentFrameBuffer::allocDepthMemory()
    {
        VkMemoryRequirements memreq;
        vkGetImageMemoryRequirements(mVkObjects.mLogicalDevice, mDepthImage, &memreq);

        VkMemoryAllocateInfo allocInfo;

        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = VK_NULL_HANDLE;
        allocInfo.allocationSize = memreq.size;
        allocInfo.memoryTypeIndex = vk::VulkanMemory::FindMemoryType(mVkObjects.mMemoryProperties, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memreq.memoryTypeBits);

        vkAllocateMemory(mVkObjects.mLogicalDevice, &allocInfo, mVkObjects.mDebugAllocCallbacks, &mDepthMemory);

        vkBindImageMemory(mVkObjects.mLogicalDevice, mDepthImage, mDepthMemory, 0U);
    }

    void PresentFrameBuffer::createDepthImage()
    {
        VkImageFormatProperties props;
        vkGetPhysicalDeviceImageFormatProperties(mVkObjects.mPhysicalDevice, VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT, VkImageType::VK_IMAGE_TYPE_2D, VkImageTiling::VK_IMAGE_TILING_OPTIMAL, VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 0U, &props);

        VkImageCreateInfo imageInfo;

        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.pNext = VK_NULL_HANDLE;
        imageInfo.flags = 0U;
        imageInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
        imageInfo.format = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
        imageInfo.extent = { mWidth, mHeight, 1U};
        imageInfo.mipLevels = 1U;
        imageInfo.arrayLayers = 1U;
        imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.queueFamilyIndexCount = 1u;
        imageInfo.pQueueFamilyIndices = &mVkObjects.mQueueFamilyIndex;
        imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED;

        vkCreateImage(mVkObjects.mLogicalDevice, &imageInfo, mVkObjects.mDebugAllocCallbacks, &mDepthImage);

        allocDepthMemory();

        VkImageViewCreateInfo imageViewInfo;

        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.pNext =  VK_NULL_HANDLE;
        imageViewInfo.flags = 0U;
        imageViewInfo.image = mDepthImage;
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

        vkCreateImageView(mVkObjects.mLogicalDevice, &imageViewInfo, mVkObjects.mDebugAllocCallbacks, &mDepthImageView);
    }

    void PresentFrameBuffer::createRenderPass()
    {
        VkAttachmentReference attach;
        attach.attachment = 0U;
        attach.layout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;

        VkAttachmentReference depthAttach;
        depthAttach.attachment = 1U;
        depthAttach.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subPass;
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

        VkAttachmentDescription descr[2];

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

        VkRenderPassCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = descr;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subPass;
        createInfo.dependencyCount = 0;
        createInfo.pDependencies = VK_NULL_HANDLE;

        VkResult result = vkCreateRenderPass(mVkObjects.mLogicalDevice, &createInfo, mVkObjects.mDebugAllocCallbacks, &mRenderPass);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Renderpass creation failed %s", vk::VkResultToString(result));
        }
    }

    void PresentFrameBuffer::createFramebuffer()
    {
        mClearValue.color.float32[0] = 0.8F;
        mClearValue.color.float32[1] = 0.6F;
        mClearValue.color.float32[2] = 0.2F;
        mClearValue.color.float32[3] = 1.0F;

        VkFramebufferCreateInfo createInfo;

        VkImageView views[] = { mExternalImageView, mDepthImageView };

        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.renderPass = mRenderPass;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = views;
        createInfo.width = mWidth;
        createInfo.height = mHeight;
        createInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(mVkObjects.mLogicalDevice, &createInfo, mVkObjects.mDebugAllocCallbacks, &mFrameBuffer);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Framebuffer creation failed %s", vk::VkResultToString(result));
        }
    }

    /*********************************************************

    **********************************************************/

    VulkanFrameBuffer::VulkanFrameBuffer(VulkanObjectContainer& vkObjects, U32 width, U32 height, U8 numAttachments, SwBool createDepth)
        : mCreateDepth(createDepth)
        , mNumAttachments(numAttachments)
        , mWidth(width)
        , mHeight(height)
        , mVkObjects(vkObjects)
        , mRenderPass(VK_NULL_HANDLE)
        , mFrameBuffer(VK_NULL_HANDLE)
        , mTextures()
        , mClearValues()
    {
        createTextures();

        createRenderPass();
        createFrameBuffer();
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
        mTextures.clear();
        vkDestroyFramebuffer(mVkObjects.mLogicalDevice, mFrameBuffer, mVkObjects.mDebugAllocCallbacks);
        vkDestroyRenderPass(mVkObjects.mLogicalDevice, mRenderPass, mVkObjects.mDebugAllocCallbacks);
    }

    U32 VulkanFrameBuffer::getNumColorAttachments() const
    {
        return 0U;
    }

    SwBool VulkanFrameBuffer::hasDepthAttachment() const
    {
        return false;
    }

    void VulkanFrameBuffer::setColorAttachmentClearColor(U32 attachmentIndex, ClearColor color)
    {
        mClearValues[attachmentIndex].color.float32[0] = color.r;
        mClearValues[attachmentIndex].color.float32[1] = color.g;
        mClearValues[attachmentIndex].color.float32[2] = color.b;
        mClearValues[attachmentIndex].color.float32[3] = color.a;
    }

    void VulkanFrameBuffer::setDepthAttachmentClearValue(F32 depthValue, U8 stencilValue)
    {
        mClearValues[0u].depthStencil.depth = depthValue;
        mClearValues[0u].depthStencil.stencil = stencilValue;
    }

    void VulkanFrameBuffer::resize(U32 width, U32 height)
    {
        mWidth = width;
        mHeight = height;

        vkDestroyFramebuffer(mVkObjects.mLogicalDevice, mFrameBuffer, mVkObjects.mDebugAllocCallbacks);
        vkDestroyRenderPass(mVkObjects.mLogicalDevice, mRenderPass, mVkObjects.mDebugAllocCallbacks);

        mTextures.clear();
        createTextures();

        createRenderPass();
        createFrameBuffer();
    }

    void VulkanFrameBuffer::createTextures()
    {
        for (uint32_t i = 0; i < mNumAttachments; i++)
        {
            /*eTextureResourceType resType = eTextureResourceType::eTextureResType_RT_color;
            VkFormat fmt = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
            bool hasDepth = (mDepthStencil == eDepthType::eDepth) || mDepthStencil == eDepthType::eDepthStencil;
            if (hasDepth && i == 0)
            {
                if (mDepthStencil == eDepthType::eDepth)
                {
                    resType = eTextureResourceType::eTextureResType_RT_depth;
                    fmt = VkFormat::VK_FORMAT_D32_SFLOAT;
                }
                else if (mDepthStencil == eDepthType::eDepthStencil)
                {
                    resType = eTextureResourceType::eTextureResType_RT_depthStencil;
                    fmt = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
                }
            }*/

            /*TextureInfo textInfo = {
                mDevice,
                mMemoryHelper,
                mWidth,
                mHeight,
                resType,
                fmt,
                false,
                VK_NULL_HANDLE,
                VK_NULL_HANDLE
            };*/

            core::Resource<VulkanTexture> newTexture = std::make_shared<VulkanTexture>(mVkObjects, mWidth, mHeight);

            //newTexture->

            mTextures.push_back(newTexture);
        }
    }

    void VulkanFrameBuffer::createRenderPass()
    {
        std::vector<VkAttachmentDescription> attachDescr;
        std::vector<VkAttachmentReference> attachRefs;
        VkAttachmentReference* depthRef = VK_NULL_HANDLE;

        //resolveAttachments(attachDescr, attachRefs, depthRef);

        VkSubpassDescription subPass;
        subPass.flags = 0;
        subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPass.inputAttachmentCount = 0;
        subPass.pInputAttachments = VK_NULL_HANDLE;
        subPass.colorAttachmentCount = static_cast<uint32_t>(attachRefs.size());
        subPass.pColorAttachments = attachRefs.data();
        subPass.pResolveAttachments = VK_NULL_HANDLE;
        subPass.pDepthStencilAttachment = depthRef;
        subPass.preserveAttachmentCount = 0;
        subPass.pPreserveAttachments = VK_NULL_HANDLE;

        /*VkSubpassDependency dep;
        dep.*/

        VkRenderPassCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.attachmentCount = mNumAttachments;
        createInfo.pAttachments = attachDescr.data();
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subPass;
        createInfo.dependencyCount = 0;
        createInfo.pDependencies = VK_NULL_HANDLE;

        vkCreateRenderPass(mVkObjects.mLogicalDevice, &createInfo, mVkObjects.mDebugAllocCallbacks, &mRenderPass);
    }

    VkClearValue createEmptyClearValue()
    {
        VkClearValue clrValue;

        clrValue.color.float32[0] = 0.0F;
        clrValue.color.float32[1] = 0.0F;
        clrValue.color.float32[2] = 0.0F;
        clrValue.color.float32[3] = 0.0F;

        clrValue.color.int32[0] = 0;
        clrValue.color.int32[1] = 0;
        clrValue.color.int32[2] = 0;
        clrValue.color.int32[3] = 0;

        clrValue.color.uint32[0] = 0U;
        clrValue.color.uint32[1] = 0U;
        clrValue.color.uint32[2] = 0U;
        clrValue.color.uint32[3] = 0U;

        clrValue.depthStencil.depth = 0.0F;
        clrValue.depthStencil.stencil = 0U;

        return clrValue;
    }

    void VulkanFrameBuffer::createFrameBuffer()
    {
        std::vector<VkImageView> views;

        mClearValues.reserve(mTextures.size());

        for (auto it : mTextures)
        {
            views.push_back(it->getView());

            VkClearValue clrValue = createEmptyClearValue();

            mClearValues.emplace_back(clrValue);
        }

        VkFramebufferCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.renderPass = mRenderPass;
        createInfo.attachmentCount = mNumAttachments;
        createInfo.pAttachments = views.data();
        createInfo.width = mWidth;
        createInfo.height = mHeight;
        createInfo.layers = 1;

        vkCreateFramebuffer(mVkObjects.mLogicalDevice, &createInfo, mVkObjects.mDebugAllocCallbacks, &mFrameBuffer);
    }

}