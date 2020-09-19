
#include "VulkanFramebuffer.hpp"
#include "VulkanShader.hpp"
#include <swizzle/core/logging/Logging.hpp>

namespace swizzle::gfx
{

    PresentFrameBuffer::PresentFrameBuffer(const VulkanObjectContainer& vkObjects, VkImage image, VkImageView imageView, VkFormat imageFormat, SwU32 width, SwU32 height)
        : mVkObjects(vkObjects)
        , mRenderPass(VK_NULL_HANDLE)
        , mFrameBuffer(VK_NULL_HANDLE)
        , mExternalImage(image)
        , mExternalImageView(imageView)
        , mImageFormat(imageFormat)
        , mClearValue()
        , mWidth(width)
        , mHeight(height)
    {
        createRenderPass();
        createFramebuffer();
    }

    PresentFrameBuffer::~PresentFrameBuffer()
    {
        vkDestroyFramebuffer(mVkObjects.mLogicalDevice, mFrameBuffer, nullptr);
        vkDestroyRenderPass(mVkObjects.mLogicalDevice, mRenderPass, nullptr);
    }

    // Inherited via FrameBuffer
    SwU32 PresentFrameBuffer::getNumColorAttachments() const
    {
        return 1U;
    }

    SwBool PresentFrameBuffer::hasDepthAttachment() const
    {
        return false;
    }

    void PresentFrameBuffer::setColorAttachmentClearColor(SwU32 attachmentIndex, ClearColor color)
    {
        attachmentIndex;
        mClearValue.color.float32[0] = color.r;
        mClearValue.color.float32[1] = color.g;
        mClearValue.color.float32[2] = color.b;
        mClearValue.color.float32[3] = color.a;
    }

    void PresentFrameBuffer::setDepthAttachmentClearValue(SwFloat depthValue, SwU8 stencilValue)
    {
        depthValue;
        stencilValue;
    }

    void PresentFrameBuffer::resize(SwU32 width, SwU32 height)
    {
        mWidth = width;
        mHeight = height;

        vkDestroyFramebuffer(mVkObjects.mLogicalDevice, mFrameBuffer, nullptr);
        vkDestroyRenderPass(mVkObjects.mLogicalDevice, mRenderPass, nullptr);

        createRenderPass();
        createFramebuffer();
    }

    core::Resource<Shader> PresentFrameBuffer::createShader(ShaderAttributeList attributeList)
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

    void PresentFrameBuffer::createRenderPass()
    {
        VkAttachmentReference attach;
        attach.attachment = 0U;
        attach.layout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;

        VkSubpassDescription subPass;
        subPass.flags = VkSubpassDescriptionFlagBits::VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX;
        subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPass.inputAttachmentCount = 0;
        subPass.pInputAttachments = VK_NULL_HANDLE;
        subPass.colorAttachmentCount = 1;
        subPass.pColorAttachments = &attach;
        subPass.pResolveAttachments = VK_NULL_HANDLE;
        subPass.pDepthStencilAttachment = VK_NULL_HANDLE;
        subPass.preserveAttachmentCount = 0;
        subPass.pPreserveAttachments = VK_NULL_HANDLE;

        subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkAttachmentDescription descr;

        descr.flags = 0;
        descr.format = mImageFormat;
        descr.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        descr.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
        descr.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
        descr.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        descr.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        descr.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        descr.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkRenderPassCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &descr;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subPass;
        createInfo.dependencyCount = 0;
        createInfo.pDependencies = VK_NULL_HANDLE;

        VkResult result = vkCreateRenderPass(mVkObjects.mLogicalDevice, &createInfo, nullptr, &mRenderPass);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Renderpass creation failed %s", VkResultToString(result));
        }
    }

    void PresentFrameBuffer::createFramebuffer()
    {
        mClearValue.color.float32[0] = 0.8F;
        mClearValue.color.float32[1] = 0.6F;
        mClearValue.color.float32[2] = 0.2F;
        mClearValue.color.float32[3] = 1.0F;


        VkFramebufferCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.renderPass = mRenderPass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &mExternalImageView;
        createInfo.width = mWidth;
        createInfo.height = mHeight;
        createInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(mVkObjects.mLogicalDevice, &createInfo, nullptr, &mFrameBuffer);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Framebuffer creation failed %s", VkResultToString(result));
        }
    }

    /*VulkanFrameBuffer::VulkanFrameBuffer(VulkanObjectContainer& vkObjects)
        : mVkObjects(vkObjects)
        , mRenderPass(VK_NULL_HANDLE)
        , mFrameBuffer(VK_NULL_HANDLE)
    {
        createRenderPass();
        createFrameBuffer();
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {

    }

    SwU32 VulkanFrameBuffer::getNumColorAttachments() const
    {
        return 0U;
    }

    SwBool VulkanFrameBuffer::hasDepthAttachment() const
    {
        return false;
    }

    void VulkanFrameBuffer::setColorAttachmentClearColor(SwU32 attachmentIndex, ClearColor color)
    {

    }

    void VulkanFrameBuffer::setDepthAttachmentClearValue(SwFloat depthValue, SwU8 stencilValue)
    {

    }

    void VulkanFrameBuffer::resize(SwU32 width, SwU32 height)
    {

    }

    void VulkanFrameBuffer::createRenderPass()
    {

    }

    void VulkanFrameBuffer::createFrameBuffer()
    {
        VkFramebufferCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.renderPass = mRenderPass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &mExternalImageView;
        createInfo.width = mWidth;
        createInfo.height = mHeight;
        createInfo.layers = 1;

        vkCreateFramebuffer(mVkObjects.mLogicalDevice, &createInfo, nullptr, &mFrameBuffer);
    }*/

}